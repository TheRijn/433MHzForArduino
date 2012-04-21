/*
 * NewRemoteSwitch library v1.0.0 DEV made by Randy Simons http://randysimons.nl/
 *
 * License: GPLv3. See license.txt
 */

#ifndef NewRemoteReceiver_h
#define NewRemoteReceiver_h

#include <Arduino.h>

struct NewRemoteCode {
	unsigned int period;		// Detected duration in microseconds of 1T in the received signal
	unsigned long address;		// Address of received code. [0..2^26-1]
	boolean groupBit;			// Group bit set or not
	unsigned short switchType;	// 0: swich off; 1: switch on; 2: set dim level
	unsigned short unit;		// Unit code of received code [0..15]
	unsigned short dimLevel;	// Dim level [0..15] iff switchType == 2
};

typedef void (*NewRemoteReceiverCallBack)(NewRemoteCode);

/**
* See RemoteSwitch for introduction.
*
* NewRemoteReceiver decodes the signal received from a 433MHz-receiver, like the "KlikAanKlikUit"-system
* as well as the signal sent by the RemoteSwtich class. When a correct signal is received,
* a user-defined callback function is called.
*
* Note that in the callback function, the interrupts are still disabled. You can enabled them, if needed.
* A call to the callback must be finished before NewRemoteReceiver will call the callback function again, thus
* there is no re-entrant problem.
*
* When sending your own code using NewRemoteSwich, disable() the receiver first.
*
* This is a pure static class, for simplicity and to limit memory-use.
*/

class NewRemoteReceiver {
	public:
		/**
		* Initializes the decoder.
		*
		* If interrupt >= 0, init will register pin <interrupt> to this library.
		* If interrupt < 0, no interrupt is registered. In that case, you have to call interruptHandler()
		* yourself whenever the output of the receiver changes, or you can use InterruptChain.
		*
		* @param interrupt 	The interrupt as is used by Arduino's attachInterrupt function. See attachInterrupt for details.
							If < 0, you must call interruptHandler() yourself.
		* @param minRepeats The number of times the same code must be received in a row before the callback is calles
		* @param callback Pointer to a callback function, with signature void (*func)(NewRemoteCode)
		*/
		static void init(short interrupt, unsigned short minRepeats, NewRemoteReceiverCallBack callback);

		/**
		* Enable decoding. No need to call enable() after init().
		*/
		static void enable();

		/**
		* Disable decoding. You can re-enable decoding by calling enable();
		*/
		static void disable();

		/**
		* Tells wether a signal is being received. If a compatible signal is detected within the time out, isReceiving returns true.
		* Since it makes no sense to transmit while another transmitter is active, it's best to wait for isReceiving() to false.
		* By default it waits for 150ms, in which a (relative slow) KaKu signal can be broadcasted three times.
		*
		* Note: isReceiving() depends on interrupts enabled. Thus, when disabled()'ed, or when interrupts are disabled (as is
		* the case in the callback), isReceiving() will not work properly.
		*
		* @param waitMillis number of milliseconds to monitor for signal.
		* @return boolean If after waitMillis no signal was being processed, returns false. If before expiration a signal was being processed, returns true.
		*/
		static boolean isReceiving(int waitMillis = 150);

		/**
		 * Called every time the signal level changes (high to low or vice versa). Usually called by interrupt.
		 */
		static void interruptHandler();

	private:

		static unsigned short _interrupt;			// Radio input interrupt
		volatile static unsigned short _state;		// State of decoding process. There are 49 states, 1 for "waiting for signal" and 48 for decoding the 48 edges in a valid code.
		static unsigned short _minRepeats;
		static NewRemoteReceiverCallBack _callback;
		static boolean _inCallback;					// When true, the callback function is being executed; prevents re-entrance.
		static boolean _enabled;					// If true, monitoring and decoding is enabled. If false, interruptHandler will return immediately.

};

#endif
