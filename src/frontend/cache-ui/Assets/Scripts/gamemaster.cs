using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Newtonsoft.Json;

public class gamemaster : MonoBehaviour
{
    public GameObject[] cpus = new GameObject[3];
    public GameObject[] cpuCache = new GameObject[3];
    public GameObject[] caches = new GameObject[3];
    public GameObject[] cacheBus = new GameObject[3];
    public GameObject bus;
    public GameObject busRam;
    public GameObject Ram;
    public string jsonData;

    private Execution execution;

    public void init(){

        execution = JsonConvert.DeserializeObject<Execution>(jsonData);

        foreach (Transition transition in execution.transitions)
        {
            Debug.Log("Count: " + transition.count);
            Debug.Log("Core: " + transition.core);
            Debug.Log("Mem Address: " + transition.mem_address);
            Debug.Log("Type: " + transition.type);

            foreach (Event e in transition.events)
            {
                Debug.Log("Event Type: " + e.event_type);
                Debug.Log("Start ID: " + e.start_id);
                Debug.Log("End ID: " + e.end_id);
                Debug.Log("Element ID: " + e.element_id); // Print element_id
                Debug.Log("Tag: " + e.tag); // Print tag
                Debug.Log("Data: " + e.data); // Print data
                Debug.Log("Target State: " + e.target_state); // Print target_state
            }
        }
    }
}


[System.Serializable]
public class Event
{
    public string event_type;
    public int start_id;
    public int end_id;
    public int element_id;
    public int tag;
    public int data;
    public string target_state;
}

[System.Serializable]
public class Transition
{
    public int count;
    public int core;
    public int mem_address;
    public string type;
    public List<Event> events;
}

[System.Serializable]
public class Execution
{
    public List<Transition> transitions;
}