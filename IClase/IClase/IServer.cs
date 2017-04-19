using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IClase
{
    public interface IServer
    {
        string Connect(string id, string port);

        string SendMessage(string Message, string id);

        string Connected();

        string Path(string DirPath);

        string KillUser(string idUser);

        Dictionary<string, string> getFollowers();

        void PrintMessages();

        void AddFollowing(string id, string port);

        Dictionary<string, string> Sender();

        string DeleteFollowing(string id);
    }
}
