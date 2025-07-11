cd arc-server

docker build --no-cache -t arc-server .
docker run -p 6969:6969 arc-server