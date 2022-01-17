BUILD_DIR="build"
OUTPUT_DIR="bin"
DEST_DIR="/usr/local/bin/"
BROOM_EXE_NAME="broom"

if [ -e $OUTPUT_DIR/$BROOM_EXE_NAME ]
then
    mv $OUTPUT_DIR/$BROOM_EXE_NAME $DEST_DIR
else
    echo "Broom not found in bin/"
fi

