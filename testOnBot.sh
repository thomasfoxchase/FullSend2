#!/bin/sh
if [ "$1" = "" ]; then
  echo "Slot number not specified. Using slot 1."
  echo "Valid slot numbers are 1-8. Try one next time. They're delicious."
  SLOT=1
else
  SLOT=$1
fi
  echo "Uploading to Slot $SLOT"
prosv5 build && prosv5 upload --slot=$SLOT && prosv5 v5 run $SLOT && prosv5 terminal
