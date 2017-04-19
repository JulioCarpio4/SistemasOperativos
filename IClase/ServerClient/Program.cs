using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.Remoting.Channels.Tcp;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting;
using System.Threading.Tasks;
using IClase;
using System.IO;
using System.Threading;
using System.Diagnostics;
using System.Text.RegularExpressions;
using System.Net;

namespace ServerClient
{
    class Program
    {

        #region Global Variables 

        static int port; //Puerto donde se monta el server. 
        //public static Dictionary<string, string> Suscripciones = new Dictionary<string, string>(); //Chats a los que este cliente está suscrito
        public static IServer remoteObject; //Objeto remoto pivote.
        static bool bandera = true;

        #endregion

        static void Main(string[] args)
        {
            try
            {
                bool banderita = true; 
                while (banderita)
                {
                    Console.WriteLine("Ingrese un número de puerto para montar el servidor:");
                    port = Convert.ToInt32(Console.ReadLine());

                    if (StartServer(port))
                    {
                        banderita = false;
                    }

                    else 
                    {
                        Console.WriteLine("Fallo al intentar levantar el servidor. Intente nuevamente");
                    }
                }
               

                Console.WriteLine("Servidor montado...");
                System.Threading.Thread.Sleep(700);
                Console.Clear();
                Console.WriteLine("PUERTO SERVER: {0}", port);
                new Thread(refreshMessages).Start();
                Console.WriteLine("**INGRESE UN COMANDO:");
                while (bandera)
                {           

                    string Comando = Console.ReadLine();

                    try
                    {
                        if (ExecuteCommand(Comando) != 1)
                        {

                            Console.WriteLine("No se pudo ejecutar el comando.");
                        }
                        System.Threading.Thread.Sleep(300);
                    }

                    catch (Exception ex)
                    {

                        Console.WriteLine("Error al procesar comando: " + ex.Message);
                    }            

                }

                Console.WriteLine("Presione <ENTER> para salir.");
                Console.ReadLine();
                Environment.Exit(0);
            }

            catch (Exception ex)
            {

                Console.WriteLine("Ocurrió un error inesperado: " + ex.Message);
                Console.ReadLine(); 
            }
        }

        //Método que descompone el comando, lo analiza sintácticamente y si es correcto lo ejecuta. 
        private static int ExecuteCommand(string comando)
        {
            //Cadena con el comando
            string [] emptyOptions = {" "};
            string[] Command = comando.Split(emptyOptions, StringSplitOptions.RemoveEmptyEntries);

            if (Command.Length == 0)
            {

                throw new Exception("No se ingresó ninguna instrucción.");
            }

            //Según la primer palabra del comando, así se validará el resto de la cadena. 
            switch (Command[0].ToUpper())
            {
                //CONNECT "IP" ":" "PUERTO" "USUARIO"
                case "CONNECT":
                    Regex rgx = new Regex(@"^\d{1,3}.\d{1,3}.\d{1,3}.\d{1,3}$");
                    if (rgx.IsMatch(Command[1]))
                    {
                        int Puerto;
                        //Se verifica que sea un puerto correcto
                        try
                        {

                            if (Command[1] == LocalIPAddress() && Convert.ToInt32(Command[3]) == port)
                            {
                                throw new Exception("No es posible conectarse a tu misma sesión de chat.");
                            }

                            Puerto = Convert.ToInt32(Command[3]);
                            //Se genera la suscripción al puerto, con la ip especificada. 
                            Subscribe1(Puerto, Command[1]);
                            //Ejecuta el método del servidor para suscribirse (+1 FOLLOWER).
                            //Clave USUARIO, Valor: IP, Puerto

                            string Answer = remoteObject.Connect(Command[4], LocalIPAddress() + "," + port);

                            if (Answer.Contains("Ya existe"))
                            {

                                throw new Exception(Answer);
                            }

                            Console.WriteLine(Answer);

                            //Guarda la suscripción en la lista propia (+1 FOLLOWING). 
                            //Clave USUARIO, Valor: IP, Puerto
                            Subscribe1(port, LocalIPAddress());
                            remoteObject.AddFollowing(Command[4], Command[1] + "," + Puerto);
                        }

                        catch (Exception ex)
                        {

                            throw new Exception(ex.Message);
                        }
                    }

                    else
                    {

                        throw new Exception("La IP ingresada no contiene el formato correcto");
                    }
                    break;
                case "LIST":

                    //Mostrar la lista de chats suscritos a mi. 
                    if (Command[1].ToUpper() == "CONNECTED")
                    {
                        Subscribe1(port, LocalIPAddress());
                        Console.WriteLine(remoteObject.Connected());
                    }

                    //Mostrar la lista de chats a los que estoy suscrito
                    else if (Command[1].ToUpper() == "SENDER")
                    {

                            Subscribe1(port, LocalIPAddress());
                            ShowFollowing(remoteObject.Sender());          
                    }

                    else if (Command[1].ToUpper() == "DIR")
                    {
                        //Obtiene la lista de chats a los que yo sigo. 
                        Subscribe1(port, LocalIPAddress());
                        Dictionary<string, string> Suscripciones = new Dictionary<string, string>();
                        Suscripciones = remoteObject.Sender();

                        if (Suscripciones.Count == 0)
                        {

                            throw new Exception("No estás conectado a ningún chat para solicitar rutas.");
                        }

                        //Manda a solicitar la información de cada path, de cada uno de los chats obtenidos. 
                        foreach (KeyValuePair<string, string> Pair in Suscripciones)
                        {
                            try
                            {

                                string[] UserData = Pair.Value.Split(',');
                                Subscribe1(Convert.ToInt32(UserData[1]), UserData[0]);
                                Console.WriteLine("\t" + Pair.Key + ": " + remoteObject.Path(Command[2]));
                            }

                            catch (Exception ex)
                            {

                                Console.WriteLine("Error al procesar el comando: " + ex.Message);
                            } 
                        }
                    }

                    else
                    {

                        throw new Exception("No se reconoce la cadena '" + Command[1] + "'");
                    }
                    break;
                case "KILL":

                    if (Command[1].ToUpper() == "USER")
                    {
                        //Obtengo el objeto de MI máquina server. 
                        Subscribe1(port, LocalIPAddress());
                        string Answer = remoteObject.KillUser(Command[2]);
                        string[] Respuesta = Answer.Split(',');
                        Subscribe1(Convert.ToInt32(Respuesta[1]), Respuesta[0]);
                        remoteObject.SendMessage("Usted ha sido removido de mi lista de suscriptores. No recibirá más mensajes de este servidor", "Servidor con Ip: " + LocalIPAddress() + ", Puerto: " + port);

                        Console.WriteLine(remoteObject.DeleteFollowing(Command[2]));


                    }

                    else
                    {
                        throw new Exception("Se esperaba 'USER'");
                    }
                    break;
                case "SEND":

                    Subscribe1(port, LocalIPAddress());
                    Dictionary<string, string> Dummy = new Dictionary<string, string>();

                    //Cambio
                    Dummy = remoteObject.Sender(); 

                    if (Dummy.Count == 0)
                    {

                        throw new Exception("No estás conectado a ningún chat para hacer un broadcast.");
                    }

                    foreach (KeyValuePair<string, string> Pair in Dummy)
                    {
                        try
                        {
                            string[] UserData = Pair.Value.Split(',');
                            Subscribe1(Convert.ToInt32(UserData[1]), UserData[0]);
                            Command[0] = "";
                            Console.WriteLine(remoteObject.SendMessage(String.Join(" ", Command), Pair.Key.ToString()));
                        }

                        catch (Exception ex)
                        {
                            Console.WriteLine("Error al procesar el comando: " + ex.Message);
                        }  
                    }

                    break;

                case "EXIT":
                    bandera = false;

                    //Se suscribe a su servidor
                    Subscribe1(port, LocalIPAddress());

                    //Obtiene su lista de followers para matarlos
                    Dictionary<string, string> Sus = remoteObject.getFollowers();
                    foreach (KeyValuePair<string, string> Pair in Sus)
                    {
                        try
                        {
                            //Mata a sus followers para que se des-suscriban
                            string Ans = "";
                            Ans = remoteObject.KillUser(Pair.Key);
                        }

                        catch (Exception ex)
                        {

                            Console.WriteLine("Error al procesar el comando: " + ex.Message);
                        }
                    }

                    //Obtiene la lista de sus suscripciones
                    Dictionary<string, string> Sen = remoteObject.Sender();

                    foreach (KeyValuePair<string, string> Pair in Sen)
                    {
                        try
                        {

                            //Obtiene la ip y puerto de la suscripcion
                            string[] UserData = Pair.Value.Split(',');

                            //Se conecta al servidor de esa suscripción
                            Subscribe1(Convert.ToInt32(UserData[1]), UserData[0]);
                            string Ans = "";

                            //Solicita a la suscripción que lo mate. 
                            Ans = remoteObject.KillUser(Pair.Key);
                        }

                        catch (Exception ex)
                        {

                            Console.WriteLine("Error al procesar el comando: " + ex.Message);
                        }
                       
                    }

                    break;

                case "CLEAR":
                    Console.Clear();
                    Console.WriteLine("PUERTO SERVER {0}", port);
                    Console.WriteLine("**INGRESE UN COMANDO:");
                    break;
                default:
                    throw new Exception("No se reconoce el comando '" + Command[0] + "' ");
            }

            return 1; 
        }

        //Método para levantar el servidor.
        private static bool StartServer(int puerto)
        {

            try
            {

                TcpChannel tcpChannel = new TcpChannel(puerto);
                ChannelServices.RegisterChannel(tcpChannel, false);

                Type commonInterfaceType = typeof(Prototype);

                RemotingConfiguration.RegisterWellKnownServiceType(commonInterfaceType, "ServerClient", WellKnownObjectMode.Singleton);
                return true;
            }

            catch 
            {

                return false;
            }
            
        }

        //Método para cambiar de objeto tonto según el puerto. 
        private static void Subscribe1(int Port, string ip)
        {
            try
            {

                Type requiredType = typeof(IServer);
                remoteObject = (IServer)Activator.GetObject(requiredType,
                "tcp://" + ip + ":" + Port + "/ServerClient");
            }

            catch (Exception ex)
            {
                throw new Exception("Error al realizar conexión con servidor en IP:" + ip + ", Puerto: " + Port + ". " + ex.Message);
            }
           
        }

        //Método para mostrar los chats a los que estoy suscrito como cliente. 
        private static void ShowFollowing(Dictionary<string, string> Suscripciones)
        {
            try
            {

                if (Suscripciones.Count == 0)
                {

                    Console.WriteLine("No te has suscrito a ningún chat");
                }

                else
                {

                    string cadena = "Se encuentra conectado a los siguientes chat como: \n\t";
                    foreach (KeyValuePair<string, string> Pair in Suscripciones)
                    {
                        string[] UserData = Pair.Value.Split(',');
                        cadena += "Usuario: " + Pair.Key + ", IP del chat: " + UserData[0] + ", Puerto del chat: " + UserData[1] + "\n\t";
                    }

                    Console.WriteLine(cadena);
                }
            }

            catch (Exception ex)
            {

                throw new Exception("Error al mostrar lista de suscripciones: " + ex.Message);
            }
        }

        //Busca en la estructura del server si hay mensajes para él aún disponibles. 
        private static void refreshMessages()
        {
            while (true)
            {
                Type requiredType = typeof(IServer);
                IServer objMensajes = (IServer)Activator.GetObject(requiredType,
                "tcp://localhost:" + port + "/ServerClient");

                objMensajes.PrintMessages();
            } 
        }

        //Obtiene la dirección IP de mi máquina. 
        public static string LocalIPAddress()
        {
            IPHostEntry host;
            string localIP = "";
            host = Dns.GetHostEntry(Dns.GetHostName());
            foreach (IPAddress ip in host.AddressList)
            {
                if (ip.AddressFamily.ToString() == "InterNetwork")
                {
                    localIP = ip.ToString();
                }
            }
            return localIP;
        }
    }
}
