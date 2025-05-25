docker build -t arc:latest .
docker run -it --name arc-container -v ./data:/app/data arc:latest