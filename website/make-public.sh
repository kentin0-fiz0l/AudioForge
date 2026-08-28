#!/bin/bash
# Make all files in Spaces bucket publicly readable

# You'll need to configure AWS CLI first with your Spaces keys
# Run: aws configure --profile digitalocean
# Then enter your Spaces access key and secret when prompted

BUCKET="audioforge-site"
REGION="sfo3"
ENDPOINT="https://${REGION}.digitaloceanspaces.com"

echo "Making all files in $BUCKET public..."

# Set ACL to public-read for all files
aws s3 sync s3://$BUCKET s3://$BUCKET \
  --endpoint-url=$ENDPOINT \
  --profile=digitalocean \
  --acl public-read \
  --recursive

echo "Done! Files should now be publicly accessible."
