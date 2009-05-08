#!/bin/sh

wget -np -nd -k --mirror http://www.daizucms.org/lua/library/oocairo/doc/
wget http://www.daizucms.org/style/block-fade.png
wget http://www.daizucms.org/style/site.css
sed -i -e "s|http://www.daizucms.org/style/site.css|site.css|g" *.html
	