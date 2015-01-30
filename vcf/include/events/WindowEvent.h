/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/

/* Generated by Together */

#ifndef WINDOWEVENT_H
#define WINDOWEVENT_H


namespace VCF{

#define WINDOW_CONST					600

#define WINDOW_EVENT_CLOSE				CUSTOM_EVENT_TYPES + WINDOW_CONST + 1
#define WINDOW_EVENT_RESTORE			CUSTOM_EVENT_TYPES + WINDOW_CONST + 2
#define WINDOW_EVENT_MAXIMIZE			CUSTOM_EVENT_TYPES + WINDOW_CONST + 3
#define WINDOW_EVENT_MINIMIZE			CUSTOM_EVENT_TYPES + WINDOW_CONST + 4

class APPKIT_API WindowEvent : public VCF::Event {
public:
	WindowEvent( Object* source );

	WindowEvent( Object* source, const unsigned long& eventType );
	
	virtual ~WindowEvent(){};

	/**
	*returns whether or not the window can close. 
	*Windows which are in their closing methods should not close 
	*if the flag has been set to false
	*/
	bool isOkToClose();

	/**
	*sets the event m_okToClose flag
	*/
	void setOkToClose( const bool& okToClose );
private:
	bool m_okToClose;
};

};
#endif //WINDOWEVENT_H