#!/usr/bin/env bash

WATCH_DIRS="./Source ./JuceLibraryCode"
JUCE_MODULES="$HOME/Programação/JUCE/modules"

echo "Watching $WATCH_DIRS for changes..."

# Trigger initial index
ctags -R --languages=C,C++ $WATCH_DIRS $JUCE_MODULES

while true; do
  inotifywait -r -e modify,create,delete,move \
    --include '\.(cpp|h|hpp|mm)$' \
    $WATCH_DIRS
  
  echo "[$(date +'%H:%M:%S')] File change detected. Updating tags..."
  ctags -R --languages=C,C++ $WATCH_DIRS $JUCE_MODULES
done
