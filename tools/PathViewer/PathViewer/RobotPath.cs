﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace PathViewer
{
    [JsonObject(MemberSerialization.OptIn)]
    public class RobotPath
    {
        [JsonProperty(PropertyName = "name")]
        public string Name;

        [JsonProperty(PropertyName = "maxvelocity")]
        public double MaxVelocity;

        [JsonProperty(PropertyName = "maxacceleration")]
        public double MaxAcceleration;

        [JsonProperty(PropertyName = "startvelocity")]
        public double StartVelocity;

        [JsonProperty(PropertyName = "endvelocity")]
        public double EndVelocity;

        [JsonProperty(PropertyName = "constraints")]
        public TimingContraint[] Constraints;

        [JsonProperty(PropertyName = "points")]
        public WayPoint[] Points;


        public RobotPath()
        {
            Points = new WayPoint[0];
            Name = string.Empty;
        }

        public RobotPath(string name)
        {
            Name = name;
            Points = new WayPoint[0];
        }

        public void AddPoint(WayPoint pt)
        {
            Array.Resize<WayPoint>(ref Points, Points.Length + 1);
            Points[Points.Length - 1] = pt;
        }

        public bool RemovePoint(WayPoint pt)
        {
            int index = Array.IndexOf(Points, pt);
            if (index == -1 || index == 0 || index == Points.Length - 1)
                return false;

            index++;
            while (index < Points.Length)
            {
                Points[index - 1] = Points[index];
                index++;
            }

            Array.Resize(ref Points, Points.Length - 1);
            return true;
        }
    }
}
