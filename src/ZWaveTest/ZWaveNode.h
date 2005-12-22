#ifndef _ZWAVE_NODE_H_
#define _ZWAVE_NODE_H_

struct nodetype
{
	unsigned char basic;
	unsigned char generic;
	unsigned char specific;
};
typedef struct nodetype NodeType;

class ZWaveNode
{
	public:
	
		/** The main constructor.
		  * @param HOME_ID
		  * @param NODE_ID
		  */
		ZWaveNode(unsigned long, unsigned short);
		
		/** Copy constructor.*/
		ZWaveNode(const ZWaveNode&);
		
		virtual ~ZWaveNode();
		
		/** Returns the node ID, range 1 - 233.*/
		virtual unsigned short nodeID() const;
		
		/** Returns the HOME_ID for a node from a Z-Wave network.
		  * Unique Node ID = HOME_ID and NODE_ID
		  */
		virtual unsigned long homeID() const;
		
		inline friend bool operator==(const ZWaveNode& left, const ZWaveNode& right)
		{
			if( left.nodeID() == right.nodeID() &&
				left.homeID() == right.homeID() )
			{
				return true;
			}
			
			return false;
		}
		
		/** Returns the node type.*/
		virtual const NodeType& type() const;
		
		/** Sets the node type.*/
		virtual void setBasicType(unsigned char);
		
		/** Sets the node type.*/
		virtual void setGenericType(unsigned char);
		
		/** Sets the node type.*/
		virtual void setSpecificType(unsigned char);
		
		/** Returns the node capabilities.*/
		virtual unsigned char capabilities() const;
		
		/** Sets the node capabilities.*/
		virtual void setCapabilities(unsigned char);
		
		/** Returns the node security.*/
		virtual unsigned char security();
		
		/** Sets the node security.*/
		virtual void setSecurity(unsigned char);
		
	private:
	
		/** Not available, force compiling error.*/
		ZWaveNode();
		
		class Private;
		Private * d;
};

#endif
