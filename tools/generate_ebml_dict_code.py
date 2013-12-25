#!/usr/bin/env python

import time
import re
import urllib2
from HTMLParser import HTMLParser

class Parser(HTMLParser):
    isSpecificationTable = False
    capture = False
    columns = []
    containsHeaders = False
    template = 'elementDefinitions.insert(std::pair<std::string, EBMLElement*>("%s", new EBMLElement(%i, (const unsigned char[]) %s)));'
    
    def convertSequence(self, s):
        out = ''
        seqBytes = s.lstrip('[').replace(']', '').split('[')
        first = True
        for seqByte in seqBytes:
            if first:
                first = False
            else:
                out += ', '
            
            out += '0x'+seqByte
        
        return '{'+out+'}'
    
    def handle_starttag(self, tag, attrs=[]):
        if tag == 'tr':
            attrs = dict(attrs)
            classes = []
            if 'class' in attrs:
                classes = attrs['class'].split(' ')
            
            self.isTopTitle = ('toptitle' in classes)
            self.columns = []
            self.containsHeaders = False
        elif tag == 'td':
            self.capture = True
        elif tag == 'th':
            self.capture = True
            self.containsHeaders = True
    
    def handle_endtag(self, tag, attrs=[]):
        if tag == 'td' or tag == 'th':
            self.capture = False
        if tag == 'tr':
            if self.containsHeaders:
                if not self.isTopTitle:
                    out = '\n// '
                    if len(self.columns) > 0:
                        out += self.columns[0]
                    print(out)
            elif len(self.columns) > 2:
                name = self.columns[0]
                sequenceArray = self.convertSequence(self.columns[2])
                sequenceLength = len(sequenceArray.split(','))
                print(self.template % (name, sequenceLength, sequenceArray))
            
    
    def handle_data(self, data):
        if self.capture:
            self.columns.append(data.strip())

url = 'http://www.matroska.org/technical/specs/index.html'

print('// code automatically generated from %s' % url)
print('// date of retrieval: %s' % time.strftime('%d %b %Y'))

res = urllib2.urlopen(url)
parser = Parser()
html = res.read()
htmlSnippet = re.sub(r'.*?<div[^>]* class="techdef"[^>]*>(.*?)<\/div>.*', r'\1', html, 1, re.MULTILINE | re.DOTALL)
parser.feed(htmlSnippet)


