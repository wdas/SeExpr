Docker images for SeExpr

Docker can be used to build development environments to validate
builds using different Linux distributions.

Centos7:

    # Build the centos7 development environment
    docker build --rm --tag seexpr:centos7 --file docker/Dockerfile.centos7 .

    # Execute a shell inside the docker environment
    docker run --rm --tty --interactive --volume "$PWD":/src --workdir /src --user "$UID:$GROUPS" seexpr:centos7

Debian:
    # Build the debian development environment
    docker build --rm --tag seexpr:debian --file docker/Dockerfile.debian .

    # Execute a shell inside the docker environment
    docker run --rm --tty --interactive --volume "$PWD":/src --workdir /src --user "$UID:$GROUPS" seexpr:debian
