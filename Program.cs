using System.Windows;

namespace ShowArgsMsgBox
{
    class Program
    {
        public static int randomFunction(string arguments)
        {
            MessageBox.Show(arguments);
            return 0;
        }
        static void Main()
        {
            // defined main
        }
    }
}
