#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#
#       This file is part of the E-Cell System
#
#       Copyright (C) 1996-2007 Keio University
#       Copyright (C) 2005-2007 The Molecular Sciences Institute
#
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#
#
# E-Cell System is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
# 
# E-Cell System is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public
# License along with E-Cell System -- see the file COPYING.
# If not, write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# 
#END_HEADER
import pygtk
pygtk.require('2.0')
import gtk
import getopt
import sys

import ModelEditor

usageString = " ModelEditor for ecell3 modeling environment.\n (C): Keio University 2003-2004 \n Authors: Gabor Bereczki <gabor.bereczki@talk21.com>, Sylvia Kinta, Dini Karnaga\n\n Usage:\n\t-f\t:\t file to load.\n\t-h\t:\t prints this help.\n"

aFileName = None
if len( sys.argv)>1:
    try:
        optstrings = getopt.getopt( sys.argv[1:], 'f:h' )[0]
    except:
        print usageString
        sys.exit(1)
        
    if len( optstrings ) > 0:
        for anOpt in optstrings:
            if anOpt[0] == '-h':
                print usageString
                sys.exit(0)
            elif anOpt[0] == '-f':
                aFileName = anOpt[1]
        if aFileName == None:
            print usageString
            sys.exit(1)
    

g=ModelEditor.ModelEditor(aFileName)
gtk.main()

