using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Threading;
namespace ConsoleApp1
{
   
    class Program
    {
        private static readonly uint FIRST_DISPLAY = 1;

        static int Main(string[] args)
        {
            
                        if (args[0] == "90")
                        {

                            //First display is 1.
                            Display.Rotate(FIRST_DISPLAY, Display.Orientations.DEGREES_CW_90);
                        }
                        else if (args[0] == "0")
                        {
                            //First display is 1.
                            Display.Rotate(FIRST_DISPLAY, Display.Orientations.DEGREES_CW_0);
                        }

                        else if (args[0] == "270")
                        {
                            //First display is 1.
                            Display.Rotate(FIRST_DISPLAY, Display.Orientations.DEGREES_CW_270);
                        }

                        else if (args[0] == "180")
                        {
                            //First display is 1.
                            Display.Rotate(FIRST_DISPLAY, Display.Orientations.DEGREES_CW_180);
                        }
            
          //  Display.Rotate(FIRST_DISPLAY, Display.Orientations.DEGREES_CW_90);
           
           // Thread.Sleep(5000); //Delay 1秒

           // Display.Rotate(FIRST_DISPLAY, Display.Orientations.DEGREES_CW_0);

            return 0;
            
        }
    }
}
