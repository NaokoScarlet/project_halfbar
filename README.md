<img width="3318" height="2034" alt="Screenshot from 2025-09-19 03-38-40" src="https://github.com/user-attachments/assets/0727d20c-4988-4ad2-82ad-85d89de1c56a" />

Project Halfbar:

This is a simple c++ program that can read raw image files and display them on the terminal.  Every character is a unicode halfblock on the bottom with a foreground color and a background color.  The character acts as the bottom pixel, and the background color acts as the top pixel.  When the image is color masked, some characters will be replaced with a top halfblock.

On Linux, you will want to install imagemagick, build-essential, and git.  git clone the repository, cd into the folder, and simply type

g++ halfbar.cpp -o halfbar

Any images you want to convert will need to be converted to raw with
convert image.png image.rgb
Then you can use
./halfbar image.rgb <image width> [ R G B ]
Where RGB is the color you want to mask.
