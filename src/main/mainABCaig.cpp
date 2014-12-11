#include <stdio.h>
#include <time.h>
#include "aig.hpp"
#include "graph.hpp"
#include "aig/saig/saig.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

extern "C"{	
	void Abc_ManStart();
}

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [The main() procedure.]

  Description [This procedure compiles into a stand-alone program for 
  DAG-aware rewriting of the AIGs. A BLIF or PLA file to be considered
  for rewriting should be given as a command-line argument. Implementation 
  of the rewriting is inspired by the paper: Per Bjesse, Arne Boralv, 
  "DAG-aware circuit compression for formal verification", Proc. ICCAD 2004.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int main( int argc, char * argv[] )
{
    // parameters
    int fUseResyn2  = 0;
    int fPrintStats = 1;
    int fVerify     = 1;
    // variables
    void * pAbc;
    char * pFileName;
    char Command[1000];
    clock_t clkRead, clkResyn, clkVer, clk;

    //////////////////////////////////////////////////////////////////////////
    // get the input file name
    if ( argc != 2 )
    {
        printf( "Wrong number of command-line arguments.\n" );
        return 1;
    }
    pFileName = argv[1];

    //////////////////////////////////////////////////////////////////////////
    // start the ABC framework
    Abc_Start();
	pAbc = Abc_FrameGetGlobalFrame();

//    if ( Cmd_CommandExecute( pAbc, Command ) )

    printf( "Reading = %6.2f sec   ",     (float)(clkRead)/(float)(CLOCKS_PER_SEC) );
    printf( "Rewriting = %6.2f sec   ",   (float)(clkResyn)/(float)(CLOCKS_PER_SEC) );
    printf( "Verification = %6.2f sec\n", (float)(clkVer)/(float)(CLOCKS_PER_SEC) );

    //////////////////////////////////////////////////////////////////////////
    // stop the ABC framework
    Abc_Stop();
    return 0;
}

