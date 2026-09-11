#!/bin/bash
# Monitor build until completion

echo "Monitoring build progress..."
echo ""

while true; do
  if grep -q "Build Summary" build-final.log 2>/dev/null; then
    echo "✅ Build complete!"
    echo ""
    tail -30 build-final.log
    break
  fi
  
  SUCCESS=$(grep -c "✅ SUCCESS" build-final.log 2>/dev/null || echo "0")
  FAILED=$(grep -c "❌" build-final.log 2>/dev/null || echo "0")
  CURRENT=$(tail -20 build-final.log | grep "Building:" | tail -1 | sed 's/.*Building: //')
  
  echo -ne "\r[$SUCCESS success, $FAILED failed] Building: $CURRENT          "
  
  sleep 5
done

echo ""
echo "Running final status check..."
./build-status.sh
