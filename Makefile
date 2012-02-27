VERSION=0.0
DATE=$(shell date +%Y-%m-%d)

all:
	cd src; make VERSION=${VERSION} DATE=${DATE}

install: all
	mkdir -p /etc/nodee /usr/local/lib/nodee
	cp src/nodee /usr/local/sbin/nodee
	cp upstart/nodee.conf /etc/init
	cp upstart/nodee-restart.conf /etc/init
	cp upstart/maybe-restart.conf /usr/local/lib/nodee
	sed -e 's/$VERSION/'${VERSION}/ \
	    -e 's/$DATE/'${DATE}/ \
	    man/nodee.8 > /usr/local/man/man8/nodee.8
