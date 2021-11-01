
~/Applications/Runway-0.19.2_4a75c01e5f25797ba354ddb98ba03e0c.AppImage & 

#cd workbench/of/apps/vscode_oF/ganthro-of-runway/

docker run --expose 9000 --network="host" -v $PWD:/workspace/ --gpus all -it my/tf1
