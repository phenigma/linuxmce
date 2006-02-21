#ifndef _ZW_SERIAL_API_H_
#define _ZW_SERIAL_API_H_

#include <deque>
#include <map>
#include <string>

#include <time.h>

#include "ZWaveNode.h"
#include "ZWaveJob.h"

using namespace std;
typedef map<int, ZWaveNode*, less<int> > NodesMap;
typedef NodesMap::iterator NodesMapIterator;
typedef NodesMap::const_iterator NodesMapCIterator;

typedef deque<ZWaveJob*> JobsDeque;
typedef JobsDeque::iterator JobsDequeIterator;
typedef JobsDeque::const_iterator JobsDequeCIterator;

/**the zwave serial api singleton class*/
class PlutoZWSerialAPI
{
	public:

		enum SerialState { STOPPED, IDLE, RUNNING, WAITTING };
		
		/**get an instance of this class
		 * @return the instance of this class*/		
		static PlutoZWSerialAPI * instance();

		virtual ~PlutoZWSerialAPI();

#ifndef _WIN32
		virtual bool start(const char *port);
#else
		virtual bool start(const char *port);
#endif
		/**listen
		 * @param timeout the listen timeout
		 * @return true if succesfull*/
		virtual bool listen(time_t timeout);
		
		virtual bool stop();
		
		virtual SerialState state() const;
		
		/**insert a job
		 * @param job the job to be inserted
		 * @return true if succesfull*/
		virtual bool insertJob(ZWaveJob* job);
		
		/**clear the jobs
		 * @return true if succesfull*/
		virtual bool clearJobs();
		
		/**insert a node
		 * @param node the node to be inserted
		 * @return true if succesfull*/
		virtual bool insertNode(ZWaveNode* node);
		
		/**removes the node with the specifid ID
		 * @param id the node id to be removed
		 * @return true if succesfull*/
		virtual bool removeNode(unsigned short id);
		
		/**get the node with the corresponding ID
		 * @param id the id of the node
		 * @return the node with the specified id*/
		virtual ZWaveNode * getNode(unsigned short id);
		
		/**get all the nodes
		 * @return the nodes*/
		virtual const NodesMap& getNodes() const;
		
		/**clear all the nodes*/
		virtual void clearNodes();
		
		/**get the home ID
		 * @return the home ID*/
		virtual unsigned long homeID() const;
		
		/**set the home ID
		 * @param homeID the home id*/
		virtual void setHomeID(unsigned long homeID);
		
		/**get the controller node ID
		 * @return the node ID*/
		virtual unsigned short nodeID() const;
		
		/**set the controller node ID
		 * @param nodeID the node ID*/
		virtual void setNodeID(unsigned short nodeID);
		
		/** get SUC id
		 * @return the SUC id*/
		virtual unsigned short sucID() const;
		
		/**set the SUC id
		 * @param sucID the SUC id*/
		virtual void setSucID(unsigned short sucID);
		
		/**sends data
		 * @param buffer the data to be sent
		 * @param length the length of the data to be sent
		 * @return true if successfull*/
		virtual bool sendData(char *buffer, size_t length);
		
		/**process the data
		 * @param buffer the data to be processed
		 * @param length the len of the data to be processed
		 * @return true if successfull*/
		virtual bool processData(const char * buffer, size_t length);

		/**get the version
		 * @return the version*/
		virtual std::string version() const;

		/**set the version
		 * param version the version*/
		virtual void setVersion(std::string& version);

		/**get the capabilities
		 * @return the capabilities*/
		virtual unsigned char capabilities() const;
		
		/**set the capabilities
		 * @param capabilities the capabilities*/
		virtual void setCapabilities(unsigned char capabilities);

		/**get the firmware version
		 * @return the firmware version*/
		virtual unsigned char firmwareVersion() const;
		
		/**sets the firmware version
		 * @param firmwareVersion the firmware version*/
		virtual void setFirmwareVersion(unsigned char firmwareVersion);
		
		/** Print some debug information about each node from the ZWave network.*/
		virtual void showZWaveNetwork() const;
		
		/** The time left until it gets timeout.*/
		virtual time_t timeLeft() const;

		private:

		PlutoZWSerialAPI();
		
		static PlutoZWSerialAPI * ref;

		class Private;
		Private * d;
};

#endif
