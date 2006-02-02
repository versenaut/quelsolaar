#
# A simple Makefile, so I don't have to do the packaging manually.
#

DIR=connector-readme

dist:
	rm -rf $(DIR) && mkdir $(DIR) && cp *.png *.html $(DIR)/ && tar czf connector-readme.tar.gz $(DIR) && rm -r $(DIR)
