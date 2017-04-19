using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO; 

namespace ServerClient
{
    class Prototype : MarshalByRefObject, IClase.IServer
    {
        public Dictionary<string,string> Followers = new Dictionary<string, string>();
        public Dictionary<string, string> Suscripciones = new Dictionary<string, string>(); //Nuevo
        public List<string> Messages = new List<string>();

        public string Connect(string id, string port)
        {
            try
            {

                if (!Followers.ContainsKey(id.ToLower()))
                {
 
                    foreach (KeyValuePair<string, string> Pair in Followers)
                    {

                        if (Pair.Value == port)
                        {

                            return "Ya existe una conexión de chat asociada a '" + port + "', con el nombre de usuario: '" + Pair.Key + "'";
                        }
                    }

                    Followers.Add(id.ToLower(), port);
                    //Nuevo
                    Messages.Add("El usuario '" + id.ToUpper() + "' se ha añadido a la sesión de chat.");
                    return "Cliente conectado exitosamente";
                }

                else
                {
                    return "Ya existe un cliente asociado con ese nombre. Intente nuevamente con un nombre de usuario distinto";
                }
            }

            catch (Exception ex)
            {
                return "Error de servidor al agregar chat: " + ex.Message;
            }
        }

        //Followers
        public string Connected()
        {
            try
            {
                if (Followers.Count == 0)
                {

                    return "No hay clientes conectados.";
                }

                string Conn = "Clientes conectados: \n\t";
                foreach (KeyValuePair<string, string> Pair in Followers)
                {
                    string[] RutaUsuario = Pair.Value.Split(',');
                    Conn += "Usuario: " + Pair.Key.ToUpper() + ", IP: " + RutaUsuario[0] + ", Puerto: " + RutaUsuario[1] + "\n\t";
                }

                return Conn; 
            }

            catch (Exception ex)
            {

                return "Error de servidor al mostrar chats suscriptores: " + ex.Message;
            }
        }

        public string KillUser(string idUser)
        {
            try
            {
                string Answer = "";

                if (Followers.Count == 0)
                {
                    throw new Exception("No existen suscriptores activos.");
                }

                if (Followers.ContainsKey(idUser.ToLower()))
                {
                    //Obtiene el puerto y la ip del suscriptor removido. 
                    Answer = Followers[idUser.ToLower()];
                    Followers.Remove(idUser.ToLower());
                    return Answer; 
                }

                else
                {

                    return "El usuario '" + idUser + "' no existe en el listado de suscritos.";
                }
                
            }

            catch (Exception ex)
            {

                return "Error de servidor al eliminar suscriptor: " + ex.Message;
            }
          
        }

        public string Path(string DirPath)
        {
            try
            {

                DirectoryInfo Directorio = new DirectoryInfo(DirPath);
                DirectoryInfo[] DirectorioSol = Directorio.GetDirectories();

                string Answer = "";
                if (Directorio.Exists)
                {
                    if (DirectorioSol.Length > 0)
                    {
                        for (int i = 0; i < DirectorioSol.Length; i++)
                        {

                            Answer += "\n\t " + (i + 1) + ". " + ((DirectoryInfo)DirectorioSol[i]).FullName;
                        }

                    }

                    else
                    {

                        Answer = "La ruta especificada no contiene ningún archivo para mostrar.";
                    }

                }

                else
                {

                    Answer = "La ruta especificada no existe.";
                }

                return Answer;
            }

            catch (Exception ex)
            {

                return "Error de servidor al buscar el path: " + ex.Message;
            }
        }

        public string SendMessage(string Message, string id)
        {
            try
            {
                Messages.Add("[" + DateTime.Now + "] " + id.ToUpper() + ":" + Message);
                return "Mensaje enviado exitosamente"; 
            }

            catch (Exception ex)
            {

                return "Error de servidor al escribir mensaje: " + ex.Message;
            }
        }

        public Dictionary<string, string> getFollowers()
        {
            return Followers;
        }

        public void PrintMessages()
        {

            if (Messages.Count != 0)
            {
                while (Messages.Count != 0)
                {

                    Console.WriteLine(Messages[0]);
                    Messages.RemoveAt(0);
                }
            }
        }

        public void AddFollowing(string id, string port)
        {

            try
            {

                Suscripciones.Add(id.ToLower(), port);
            }

            catch (Exception ex)
            {

                throw new Exception(ex.Message);
            }
        }

        public Dictionary<string, string> Sender()
        {
            return Suscripciones; 
        }

        public string DeleteFollowing(string id)
        {
            try
            {

                string Answer = "";

                if (Suscripciones.ContainsKey(id.ToLower()))
                {

                    Suscripciones.Remove(id.ToLower());
                    Answer = "Se ha removido la suscripción de manera exitosa.";
                }

                else
                {

                    Answer = "El usuario '" + id +"' no existe en la lista de chats.";

                }

                return Answer;
            }

            catch (Exception ex)
            {

                return "Error de servidor al remover suscriptor: " + ex.Message;
            }
        }
    }
}
