# Build using docker

```
docker build --tag slang .
rm -rf build
docker run -d -v $(pwd):/slang -ti --name slang_dev slang:dev /bin/bash
docker exec -ti slang_dev /bin/bash
```

Once inside the container:
```
scripts/build_clang.sh
```

