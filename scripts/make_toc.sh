#!/bin/sh

find docs -name "../docs/*.md" -type f -print0 | xargs -0 -I {} markdown-toc -i {}
