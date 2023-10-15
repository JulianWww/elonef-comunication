mkdir package
cp -r lib/* package/
cp -r lib package/

cp package.json package/
tar -czvf package.tar.gz package