from EditorObject import *
from Constants import *
from ShapeDescriptor import *
from LayoutCommand import *
from Utils import *
from ComplexLine import *

class ConnectionObject( EditorObject ):
	
	def __init__( self, aLayout,objectID, aVariableID, aProcessID, aVariableRing, aProcessRing, aVarrefName, parentSystem ):
		EditorObject.__init__( self, aLayout, objectID, 0, 0, parentSystem)
		self.thePropertyMap[ CO_PROCESS_RING ] = aProcessRing
		self.thePropertyMap[ CO_VARIABLE_RING ] = aVariableRing

		self.thePropertyMap[ OB_HASFULLID ] = False
		self.thePropertyMap [ OB_SHAPE_TYPE ] = SHAPE_TYPE_STRAIGHT_LINE
		self.thePropertyMap [ CO_LINEWIDTH ] = 3
		self.thePropertyMap[ OB_TYPE ] = OB_TYPE_CONNECTION
		self.thePropertyMap [ OB_FILL_COLOR ] = self.theLayout.graphUtils().getRRGByName("black")
		#default dimensions
		# get label from processID
		processObj = self.theLayout.getObject( aProcessID )
		aFullID = processObj.getProperty( OB_FULLID )
		aVarrefList = self.theLayout.theLayoutManager.theModelEditor.getModel().getEntityProperty( createFullPN( aFullID, ME_PROCESS_VARREFLIST ) )
		aCoef = 0
		for aVarref in aVarrefList:
			if aVarref[ MS_VARREF_NAME ] == aVarrefName:
				aCoef = aVarref[ MS_VARREF_COEF ]


		self.thePropertyMap[ CO_NAME ] = aVarrefName
		self.thePropertyMap[ CO_COEF ] = aCoef
		self.thePropertyMap[ CO_USEROVERRIDEARROW ] = False
		if type(aProcessID) == type( [] ): #CANNOT BE!!!
			self.thePropertyMap[ CO_PROCESS_ATTACHED ] = None
			self.thePropertyMap[ CO_ENDPOINT1 ] = aProcessID
			self.thePropertyMap[ CO_ATTACHMENT1TYPE ] = OB_NOTHING
			self.thePropertyMap[ CO_DIRECTION1 ] = self.__getRingDirection( RING_TOP )
		else:
			self.thePropertyMap[ CO_PROCESS_ATTACHED ] = aProcessID
			aProcessObj = self.theLayout.getObject( aProcessID )
			aProcessObj.registerConnection( objectID )
			(x, y) = aProcessObj.getRingPosition( aProcessRing )
			rsize = aProcessObj.getRingSize()
			self.thePropertyMap[ CO_ENDPOINT1 ] = [ x +rsize/2, y+rsize/2 ]
			self.thePropertyMap[ CO_ATTACHMENT1TYPE ] = OB_TYPE_PROCESS
			aProcessFullID = aProcessObj.getProperty( OB_FULLID )
			aModelEditor = self.theLayout.theLayoutManager.theModelEditor
			aVarrefList = aModelEditor.getModel().getEntityProperty( aProcessFullID + ':' + ME_PROCESS_VARREFLIST )
			for aVarref in aVarrefList:
				if aVarref[ MS_VARREF_NAME ] == aVarrefName:
					self.thePropertyMap[ CO_COEF ] = aVarref[ MS_VARREF_COEF ]
					break
			self.thePropertyMap[ CO_DIRECTION1 ] = self.__getRingDirection( aProcessRing )
			
		if type(aVariableID) == type( [] ):
			self.thePropertyMap[ CO_VARIABLE_ATTACHED ] = None
			self.thePropertyMap[ CO_ENDPOINT2 ] = aVariableID
			self.thePropertyMap[ CO_ATTACHMENT2TYPE ] = OB_NOTHING
			self.thePropertyMap[ CO_DIRECTION2 ] = self.__getRingDirection( RING_BOTTOM)
		else:
			self.thePropertyMap[ CO_VARIABLE_ATTACHED ] = aVariableID
			aVariableObj = self.theLayout.getObject( aVariableID )
			(x, y) = aVariableObj.getRingPosition( aVariableRing )
			self.thePropertyMap[ CO_ENDPOINT2 ] = [x, y]
			self.thePropertyMap[ CO_ATTACHMENT2TYPE ] = OB_NOTHING
			self.thePropertyMap[ CO_DIRECTION2 ] = self.__getRingDirection( aVariableRing)
                        self.thePropertyMap[ CO_ATTACHMENT2TYPE ] = OB_TYPE_VARIABLE
			aVariableObj.registerConnection( objectID )
		self.__defineArrowDirection()


		aLineSD = StraightLineSD(self, self.getGraphUtils() )

		self.theSD = aLineSD
		self.thePropertyMap[ OB_SHAPEDESCRIPTORLIST ] = aLineSD
		self.theConnectionArrowTypeList=['Straight','Cornered']
		self.theConnectionLineTypeList=['Normal', 'Bold', 'Dashed' ,'Dotted']

	def __defineArrowDirection( self ):
		self.thePropertyMap[ CO_HASARROW1 ] = gtk.FALSE
		self.thePropertyMap[ CO_HASARROW2 ] = gtk.FALSE
		
		if self.thePropertyMap[ CO_COEF ] <0:
			self.thePropertyMap[ CO_HASARROW1 ] = gtk.TRUE
		elif self.thePropertyMap[ CO_COEF ] >0:
			self.thePropertyMap[ CO_HASARROW2 ] = gtk.TRUE


	def parentMoved( self, parentID, deltax, deltay ):
		if parentID ==self.thePropertyMap[ CO_VARIABLE_ATTACHED ]:
			changedType = OB_TYPE_VARIABLE
		else:
			changedType = OB_TYPE_PROCESS
		if changedType == self.thePropertyMap[ CO_ATTACHMENT1TYPE ]:
			chgProp = CO_ENDPOINT1
		else:
			chgProp = CO_ENDPOINT2
		(x, y) = self.thePropertyMap[ chgProp ]
		x+=deltax
		y+=deltay
		self.thePropertyMap[ chgProp ] = [x, y]
		self.theShape.repaint()


	def __getRingDirection( self, ringCode ):
		if ringCode == RING_TOP:
			return DIRECTION_UP
		elif ringCode == RING_BOTTOM:
			return DIRECTION_DOWN
		elif ringCode == RING_LEFT:
			return DIRECTION_LEFT
		elif ringCode == RING_RIGHT:
			return DIRECTION_RIGHT


	def reconnect( self ):
		pass

	def setProperty(self, aPropertyName, aPropertyValue):
		self.thePropertyMap[aPropertyName] = aPropertyValue
		if aPropertyName == OB_SHAPE_TYPE:
			if  self.theCanvas !=None:
				self.theShape.delete()
				self.getArrowType(aPropertyValue)
				self.theShape.show()
		elif aPropertyName == CO_NAME:
			if  self.theCanvas !=None:
				self.theShape.reName()
		elif aPropertyName == CO_COEF:
			self.__defineArrowDirection()
			if  self.theCanvas !=None:
				self.theShape.repaint()
		elif aPropertyName == CO_LINETYPE:
			pass
		elif aPropertyName == CO_LINEWIDTH:
			pass
		elif aPropertyName == CO_HASARROW1:
			pass
		elif aPropertyName == CO_HASARROW2:
			pass
		EditorObject.setProperty(self, aPropertyName, aPropertyValue)


	def getAvailableArrowType(self):
		return self.theConnectionArrowTypeList


	def getAvailableLineType(self):
		return self.theConnectionLineTypeList

	def getArrowType(self, aShapeType):
		if aShapeType == SHAPE_TYPE_STRAIGHT_LINE:
			aLineSD = StraightLineSD(self, self.getGraphUtils() )
		elif aShapeType == SHAPE_TYPE_CORNERED_LINE:
			aLineSD = corneredLineSD(self, self.getGraphUtils() )

		self.theSD = aLineSD
		self.thePropertyMap[ OB_SHAPEDESCRIPTORLIST ] = aLineSD

	def show( self ):
		self.theShape = ComplexLine( self, self.theCanvas )
		self.theShape.show()

	
	def checkConnections( self, end = 2 ):
		# get position of arrowhead 2
		queryProp = CO_ENDPOINT2
		attProp = CO_ATTACHMENT2TYPE

		if end ==1:
			queryProp = CO_ENDPOINT1
			attProp = CO_ATTACHMENT1TYPE
		(x,y) = self.thePropertyMap[ queryProp ]
		currentAttachment = self.thePropertyMap[ attProp ]
		if currentAttachment == OB_TYPE_PROCESS:
			checkFor = OB_TYPE_PROCESS
		else:
			checkFor = OB_TYPE_VARIABLE
		( aShapeID, aRingID ) = self.theLayout.checkConnection( x, y, checkFor )


	def destroy(self):
		EditorObject.destroy( self )
		varID = self.thePropertyMap[ CO_VARIABLE_ATTACHED ]
		if varID != None:
			self.theLayout.getObject( varID ).unRegisterConnection( self.theID )
		procID = self.thePropertyMap[ CO_PROCESS_ATTACHED ]
		self.theLayout.getObject( procID ).unRegisterConnection( self.theID )
		