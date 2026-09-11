#!/bin/bash
while true; do
  if tail -20 build-final.log | grep -q "Build Summary"; then
    echo "Build completed!"
    tail -50 build-final.log
    break
  fi
  
  # Show current plugin
  CURRENT=$(tail -50 build-final.log | grep "Building:" | tail -1)
  STATUS=$(tail -10 build-final.log | grep -E "(SUCCESS|failed)" | tail -1)
  
  if [ ! -z "$CURRENT" ]; then
    echo "$CURRENT"
    if [ ! -z "$STATUS" ]; then
      echo "  $STATUS"
    fi
  fi
  
  sleep 5
done
