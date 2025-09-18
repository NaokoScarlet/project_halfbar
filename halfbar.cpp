#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <sstream>

struct Image
{
  int size {};    // File size
  int h {};       // Horizonal
  int v {};       // Vertical
  std::vector<unsigned char> m {0, 0, 0}; // Mask value
  bool isMasked { 0 };  // Enable or disable masking based on command line arguments
};

std::string getArr(std::vector<unsigned char>& arr, int pos, char i)
{
  std::string buf {};
  for (int x {0}; x < 3; x++)
  buf+=i+std::to_string(arr[(pos*3)+x]);
  return buf;
}

std::string getCol(std::vector<unsigned char>& rgb,int pos, int x)
{ // Output colors, Usage: getCol(imgdata,position,switch)
  switch (x)
  {
    case 0:  // Returns foreground color
      return "\e[38;2" + getArr(rgb,pos,';') + 'm';
    case 1:  // Returns background color
      return "\e[48;2" + getArr(rgb,pos,';') + 'm';
    default:
      return "";
  }
}

std::string defCol(int x)
{ // Reset colors to default
  switch (x)
  {
    case 0: // Foreground
      return "\e[39m";
    case 1: // Background
      return "\e[49m";
    case 2: // Both foreground and background
      return "\e[39m\e[49m";
    default:
      return "";
  }
}

bool isMask (std::vector<unsigned char>& rgb, Image& img, int pos)
{ // Check Mask in pixel
  for (int x {0}; x < 3; x++)
  {
    if ( rgb[((pos*3)+x)] != img.m[x] )
    {
      return 0;
    }
  }
      return 1;
}

std::string genPx(std::vector<unsigned char>& rgb, Image& img, int pos)
{ // Generate top and bottom pixel in one character
  // Top pixel is background color, and bottom pixel is foreground color
  // with unicode half block on the bottom which is \u2548

  // Check if image has mask
  if (!img.isMasked)
  return getCol(rgb,(pos-1),1) + getCol(rgb,((pos+img.h)-1),0) + "\u2584";

  // mask in both pixels
  if ( isMask(rgb,img,(pos-1)) && isMask(rgb,img,((pos+img.h)-1) ))
  return defCol(2) + " ";

  // mask in top pixel
  if ( isMask(rgb,img,(pos-1)) && !isMask(rgb,img,((pos+img.h)-1) ))
  return defCol(1) + getCol(rgb,((pos+img.h)-1),0) + "\u2584";

  // mask in bottom pixel
  if ( !isMask(rgb,img,(pos-1)) && isMask(rgb,img,((pos+img.h)-1) ))
  return defCol(1) + getCol(rgb,(pos-1),0) + "\u2580";
  // When the bottom pixel is masked, top pixel becomes forground color,
  // then unicode halfblock is changed from bottom to top, which is \u2580

  // No mask in pixels
  return getCol(rgb,(pos-1),1) + getCol(rgb,((pos+img.h)-1),0) + "\u2584";
}

std::string genPx_t(std::vector<unsigned char>& rgb,Image& img, int pos)
{ // Generate pixels for top line on images with odd vertical resolution

  // Check if image masking is enabled
  if (!img.isMasked)
  return defCol(1) + getCol(rgb,(pos-1),0) + "\u2584";

  // Mask in pixel
  if (isMask(rgb,img,(pos-1)))
  return defCol(2) + " ";

  return defCol(1) + getCol(rgb,(pos-1),0) + "\u2584";

}

std::stringstream bufLine(std::vector<unsigned char>& imgdata, Image& img, int pos)
{ // Convert two lines in image
  std::stringstream buf {};
  for (int x {1}; x <= img.h; x++)
  {
    buf << genPx(imgdata,img, (x+(pos*img.h)) );
  }
  buf << defCol(2) << " \n";  // Space is needed at end to prevent terminal
                              // from showing last pixel on right, when terminal
                              // width is less than image width
  return buf;
}

std::stringstream bufLine_t(std::vector<unsigned char>& imgdata, Image& img)
{ // Convert top line for images with odd vertical resolution
  std::stringstream buf {};
  for (int x {1}; x <= img.h; x++)
  {
    buf << genPx_t(imgdata,img,x);
  }
  buf << defCol(2) << " \n";  // Space is needed at end to prevent terminal
                              // from showing last pixel on right, when terminal
                              // width is less than image width
  return buf;
}


std::stringstream hbImage(std::vector<unsigned char>& imgdata, Image& img)
{ // convert image to terminal pixels
  std::stringstream buffer {};
  buffer << "\e[?7l"; // Prevent image from wrapping in terminal

  if (img.v%2 == 0)
  { // Check if image is even
    for (int x {0}; ( x+1) < img.v; x+=2)
    {
      buffer << bufLine(imgdata,img,x).str();
    }
  }
  else
  { // If image is odd, generate top line with bottom pixels
    buffer << bufLine_t(imgdata,img).str();
    for (int x {1}; ( x+1) < img.v; x+=2)
    {
      buffer << bufLine(imgdata,img,x).str();
    }
  }

  buffer << "\e[?7h"; // Enable wrapping in terminal after image is printed
  return buffer;
}

int main(int argc,char* argv[])
{
  std::ifstream inFile { argv[1], std::ios::binary };
  if (!inFile.is_open())
  { // Check if file loads
    std::cerr << "Error opening file.\n";
    return 1;
  }

  if (argc < 3)
  { // Check arguments
    std::cerr << "Usage: " << argv[0] << " <image> <width> (r g b)\n";
    return 1;
  }

  Image img {}; // Load image info
  img.size = std::filesystem::file_size(argv[1]); // Filesize
  std::stringstream (argv[2]) >> img.h;           // Width
  img.v = ((img.size/3)/img.h);                   // Height

  if( !((img.size/3)%img.h == 0) )
  { // Check width
    std::cerr << "Invalid Width\n";
    return 1;
  }

  //load image
  std::vector<unsigned char> imgdata(img.size);
  inFile.read(reinterpret_cast<char*> (imgdata.data()), img.size);

  if (argc >= 6)
  { // Check for opacity values in argument
    int buf {};
    img.isMasked = 1; // Set Masking option to true
    for (int x {0}; x < 3; x++)
    {
      std::stringstream(argv[(x+3)]) >> buf;
      img.m[x] = buf; // Assign Values
    }
  }

  // Print image onto terminal
  std::cout << hbImage(imgdata,img).str();

  return 0;
}
