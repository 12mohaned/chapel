# bash shell script to set the Chapel environment variables


# shallow test to see if we are in the correct directory
path_tail=`pwd | sed 's/.*\///g'`
if [ "$path_tail" != "chapel" ] 
   then
      echo "Error: You muse use '. util/setchplenv' from within the chapel directory"
   else
      echo "Setting CHPL_HOME..."
      CHPL_HOME=`pwd`
      export CHPL_HOME
      echo "                    ...to $CHPL_HOME"
      echo " "

      echo "Setting CHPL_PLATFORM..."
      CHPL_PLATFORM=`"$CHPL_HOME"/util/platform`
      export CHPL_PLATFORM
      echo "                        ...to $CHPL_PLATFORM"
      echo " "

      echo "Updating PATH to include..."
      PATH="$PATH":"$CHPL_HOME"/bin/$CHPL_PLATFORM
      export PATH
      echo "                           ...$CHPL_HOME"/bin/$CHPL_PLATFORM
      echo " "
fi
