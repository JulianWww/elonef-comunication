mkdir client
cp -r src client/
cp -r node_modules client/
cp * client

echo "export class ServerConnectionHandler {}" > client/src/handlers/serverConnectionHandler.ts
jq '.name |= "\(.)-client"' package.json > client/package.json
cd client

yarnpkg build
browserify lib/index.js -r crypto -o lib/index.js


./make_package.sh
cd ..