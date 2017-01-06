using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace MultidumperGUI
{
    public class Containerinfo
    {

        [JsonProperty("copyright")]
        public string Copyright { get; set; }

        [JsonProperty("dumper")]
        public string Dumper { get; set; }

        [JsonProperty("game")]
        public string Game { get; set; }

        [JsonProperty("system")]
        public string System { get; set; }
    }

    public class Song
    {

        [JsonProperty("author")]
        public string Author { get; set; }

        [JsonProperty("comment")]
        public string Comment { get; set; }

        [JsonProperty("name")]
        public string Name { get; set; }
    }

    public class MDOut
    {

        [JsonProperty("channels")]
        public IList<string> Channels { get; set; }

        [JsonProperty("containerinfo")]
        public Containerinfo Containerinfo { get; set; }

        [JsonProperty("songs")]
        public IList<Song> Songs { get; set; }

        [JsonProperty("subsongCount")]
        public int SubsongCount { get; set; }
    }

}
