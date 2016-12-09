//============================================================================
// TwsApi Empty
//============================================================================
#include "TwsApiL0.h"
#include "TwsApiDefs.h"
using namespace TwsApi;

// to use the Sleep function
#include <unistd.h>			// usleep(), in microseconds
#define Sleep( m ) usleep( m*1000 )

class MyEWrapper: public EWrapperL0
{
public:
    MyEWrapper(bool CalledFromThread = true): EWrapperL0(CalledFromThread) {}
    
    virtual void connectionOpened( void )
    {
        printf( "Connection Opened\n");
    }
    
    virtual void connectionClosed()
    {
        printf( "Connection Closed\n");
    }
    
    virtual void managedAccounts( const IBString &accountsList )
    {
        const IBString &al = accountsList;
        printf("MA: %s\n", (const char*)al);
    }
    
    virtual void nextValidId( OrderId orderId )
    {
        printf( "nextValidId = %ld\n", orderId );
    }
    


    virtual void contractDetails( int reqId, const ContractDetails& contractDetails )
    {
        const Contract &C = contractDetails.summary;
        //const IBString &T = contractDetails.liquidHours;
        //const IBString &Tz = contractDetails.timeZoneId;
        
        printf( "%s,%5.2f,%s,%s\n", (const char*)C.expiry, C.strike, (const char*)C.right, (const char*)C.localSymbol );
        //printf( "CD: %s in %s\n", (const char*)T, (const char*)Tz);
    }
    
    virtual void contractDetailsEnd( int reqId )
    {
        printf("CD: %i end\n", reqId);
    }

    virtual void winError( const IBString& str, int lastError )
    {
        printf( "WinError: %d = %s\n", lastError, (const char*)str );
    }
    
    virtual void error( const int id, const int errorCode, const IBString errorString )
    {
        printf( "Error for id=%d: %d = %s\n", id, errorCode, (const char*)errorString );
    }
};

struct Contract_ : public Contract
{
    Contract_(IBString sb, IBString st, IBString cr, IBString ex, IBString pe): Contract()
    {
        symbol = sb;
        secType = st;
        currency = cr;
        exchange = ex;
        primaryExchange = pe;
    }
};

int main( void )
{
    Contract_ C("SPY", *SecType::OPT, "USD", *Exchange::IB_SMART, *Exchange::ARCA);

    printf("API Version = %s\n", EClientL0::apiVersion() );
    
    MyEWrapper	MW;
    EClientL0 *EC = EClientL0::New( &MW );
    
    printf("ClientVersion = %d\n", EC->clientVersion() );
    
    EC->eDisconnect();
    
    if (EC->eConnect("", 4001, 100))
    {
        printf("ServerVersion = %d\n", EC->serverVersion() );
        EC->reqContractDetails(1, C);
    }
    
    Sleep(10000);
    
    EC->eDisconnect();
    
    delete EC;
    return 0;
}
