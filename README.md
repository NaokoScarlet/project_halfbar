<img width="3318" height="2034" alt="Screenshot from 2025-09-19 03-38-40" src="https://github.com/user-attachments/assets/0727d20c-4988-4ad2-82ad-85d89de1c56a" />

Project Halfbar:

This is a simple c++ program that can read raw image files and display them on the terminal.  Every character is a unicode halfblock on the bottom with a foreground color and a background color.  The character acts as the bottom pixel, and the background color acts as the top pixel.  When the image is color masked, some characters will be replaced with a top halfblock.

On Linux, you can install imagemagick, build-essential, and git.  git clone the repository, cd into the folder, and simply type

g++ halfbar.cpp -o halfbar

Any images you want to convert will need to be converted to raw with

convert image.png image.rgb

Then you can use

./halfbar image.rgb width '[R G B]'

Where RGB is the color you want to mask.
You can also output to a file with

./halfbar image.rgb width '[R G B]' >image.hbi

You can make it a shell executable with

echo 'echo -n "'"$(cat image.hbi)"'"' > image.sh

chmod +x image.sh

If you want to be greeted by the image in an SSH login, you can add the shell script to .profile in your home directory.

Since this is my first project on github I've decided not to include a binary.  I'm leaving it up to the user to review the code before compiling and running the binary.  Any issues brought up may or may not be given attention, depending on the issue.  I'm still new to c++ and may be focusing on other projects.
