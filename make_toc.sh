#!/bin/sh

find docs -name "*.md" -type f -print0 | xargs -0 -I {} markdown-toc -i {}
