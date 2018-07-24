# Build using docker

Build the image with the development environment
```
docker build --tag slang:master .
```

Launch the container with the runme script, this will get the latest master failed from github,
compile them and run the unit tests:

```
docker run -it --rm slang:master ./runme
```

