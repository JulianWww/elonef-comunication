mkdir client
cp -r src client/
cp -r node_modules client/
cp * client

echo "export class ServerConnectionHandler {}" > client/src/handlers/serverConnectionHandler.ts
jq '.name |= "\(.)-client"' package.json > client/package.json