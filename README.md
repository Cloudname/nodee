# What is it?

Nodee is a daemon that runs on each node in a cloud environment and
manages software artifacts and service lifecycle.

# Notes.

As mentioned above, Nodee has two main responsibilities.

## Managing software artifacts.

Nodee maintains a directory tree containing the software artifacts
needed to run a service.  The structure of the tree looks a bit like
that of a Maven repository.

> /prefix/software/groupId/artifact/version

When installing artifacts into the above tree structure, Nodee fetches
a ZIP file containing the artifact and then unpacks the artifact into
the above directory structure.  The software artifact is usually
served from an HTTP server that has a directory structure similar to
the above directory structure:

> http://artifact.server.com/sw/<groupId>/<artifact>/<version>/<filename>.zip

To manage software Nodee supports the following operations:

* install artifact
* remove artifact
* list installed artifacts

## Managing service lifecycles

Nodee has the capability to start services and monitor the resulting
processes to ensure that the process is running.  Kind of like the
init process in a UNIX system.

To start a service Nodee will be told the following:

* The service coordinate
* The software artifact
* The options we wish to pass to the service

When starting a service, Nodee will first create a runtime directory
for the service.  The runtime directory is derived from the Coordinate
of the service.  So for a service with the coordinate
"1.myservice.borud.fbu" the runtime directory will be:

> /prefix/runtime/fbu/borud/myservice/1

Services are **not** allowed to modify anything under /prefix/software.

## Notes

### Tidyness

Services should be implemented in a manner that allows them to keep
their runtime state in the runtime directory.  The software artifact
in /prefix/software should be viewed as immutable. 

Some services may require their runtime directory to be pre-populated
with files before startup.  Nodee should provide mechanisms for this.
This can either be derivatives of files that are in the
/prefix/software tree or it can be saved state that needs to be
fetched from somewhere else (like an S3 backup etc).

### Integration with Cloudname

When Nodee starts up it must make its presence known through
Cloudname.  We intend to have a separate part of the cloudname
namespace in ZooKeeper set aside for discovering Nodee instances, and
thereby instances that are available for running services.  The
current thinking is that each Nodee instance is responsible for
maintaining an ephemeral node under:

> /meta/cell/nodes/<instance name>

Where the *instance name* is equal to the machine name (for instance
the Amazon EC2 instance name).

The data Nodee populates into this ephemeral node is information about
the machine instance itself, its status, what services are running on
it and where its web console is running (what port).


## Networked API

Nodee will have a REST API that allows management utilities to query
Nodee for its status and ask it to install/uninstall software
artifacts and start/drain/stop services.
