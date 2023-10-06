using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Newtonsoft.Json;
using TMPro;
using Unity.VisualScripting;
using Unity.Collections;

public class Gamemaster : MonoBehaviour
{
    public Cpu[] cpus = new Cpu[3];
    public Arrows[] cpuCache = new Arrows[3];
    public Cache[] caches = new Cache[3];
    public Arrows[] cacheBus = new Arrows[3];
    public Bus bus;
    public Arrows busRam;
    public Ram ram;
    public string jsonData;
    public bool run;
    public TextMeshProUGUI log;

    private Execution execution;
    private int[] regs = new int[3];
    private string protocol;

    void Start()
    {

        GameObject[] cpuObjects = GameObject.FindGameObjectsWithTag("CPU");

        for (int i = 0; i < cpuObjects.Length; i++)
        {   
            Cpu cpuComponent = cpuObjects[i].GetComponent<Cpu>();

            char lastChar = cpuObjects[i].name[^1];

            int index = int.Parse(lastChar.ToString());

            cpus[index] = cpuComponent;

        }

        GameObject[] cpuCacheObjects = GameObject.FindGameObjectsWithTag("CPU_CACHE");

        for (int i = 0; i < cpuCacheObjects.Length; i++)
        {
            Arrows cpuCacheComponent = cpuCacheObjects[i].GetComponent<Arrows>();

            char lastChar = cpuCacheObjects[i].name[^1];

            int index = int.Parse(lastChar.ToString());

            cpuCache[index] = cpuCacheComponent;
        }

        // Assign Cache components based on the last character of GameObject names
        GameObject[] cacheObjects = GameObject.FindGameObjectsWithTag("CACHE");

        for (int i = 0; i < cacheObjects.Length; i++)
        {
            Cache cacheComponent = cacheObjects[i].GetComponent<Cache>();

            char lastChar = cacheObjects[i].name[^1];

            int index = int.Parse(lastChar.ToString());

            caches[index] = cacheComponent;
        }

        // Assign Cache Bus components based on the last character of GameObject names
        GameObject[] cacheBusObjects = GameObject.FindGameObjectsWithTag("CACHE_BUS");

        for (int i = 0; i < cacheBusObjects.Length; i++)
        {
            Arrows cacheBusComponent = cacheBusObjects[i].GetComponent<Arrows>();

            char lastChar = cacheBusObjects[i].name[^1];

            int index = int.Parse(lastChar.ToString());
            
            cacheBus[index] = cacheBusComponent;
        }

        for (int i = 0; i < 3; i++)
        {
            regs[i] = 0;
        }

        bus = GameObject.FindGameObjectWithTag("BUS").GetComponent<Bus>();

        busRam = GameObject.FindGameObjectWithTag("BUS_RAM").GetComponent<Arrows>();

        ram = GameObject.FindGameObjectWithTag("RAM").GetComponent<Ram>();

    }
    public void Init()
    {

        protocol = "MESI" ;
        execution = JsonConvert.DeserializeObject<Execution>(jsonData);

        log.text += "STARTING EMULATION WITH PROCOTOL " + protocol + "";

        StartCoroutine(ProcessTransactions());
    }

    IEnumerator ProcessTransactions()
    {

        float wait = 1.0f;
        execution = JsonConvert.DeserializeObject<Execution>(jsonData);

        foreach (Transaction transaction in execution.transactions)
        {

            // Log new instruction
            log.text += "\nCore " + transaction.core + " " + transaction.type + "\n";

            if (transaction.type == "INC")
            {
                cpus[transaction.core].ChangeInstruction("INC R0");
                regs[transaction.core]++;
                yield return new WaitForSeconds(wait);
                cpus[transaction.core].ChangeRegister("R0 : " + regs[transaction.core]);

            }

            else
            {
                string newInst = transaction.type + " 0x" + transaction.mem_address.ToString("X");    
                cpus[transaction.core].ChangeInstruction(newInst);
                yield return new WaitForSeconds(wait);
                cpuCache[transaction.core].MoveDown();
            }

            yield return new WaitForSeconds(wait);

            foreach (Event e in transaction.events)
            {

                if (e.event_type == "edit")
                {

                    string newText = "";
                    string state = "";

                    switch (e.target_state)
                    {
                        case "Modified":
                            state = "M";
                            break;

                        case "Owned":
                            state = "O";
                            break;

                        case "Exclusive":
                            state = "E";
                            break;

                        case "Shared":
                            state = "S";
                            break;

                        case "Invalid":
                            state = "I";
                            break;

                        default:
                            break;
                    }

                    if (e.element_id == -1) 
                    {
                        newText = "0x" + e.tag.ToString("X") + " : " + e.data;
                        log.text += "       RAM was written " + e.data + " on address 0x" + e.tag.ToString("X") + "\n";
                        ram.ChangeLine(newText, e.tag);
                    }
                    else
                    {
                        newText = "0x" + e.tag.ToString("X") + " : " + e.data + " : " + state;
                        log.text += "       Cache " + e.element_id + " has written " + e.data + " on line " + (e.tag % 4).ToString("X") + "\n";
                        log.text += "       Line chaged to state " + state + "\n";
                        caches[e.element_id].ChangeLine(newText, e.tag % 4);
                    }

                }

                else if (e.event_type == "move")
                {
                    
                    if (e.start_id == -1){
                        busRam.MoveUp();
                        
                    }
                    else
                    {
                        cacheBus[e.start_id].MoveDown();
                    }

                    yield return new WaitForSeconds(wait);

                    bus.Move(e.start_id,e.end_id);

                    yield return new WaitForSeconds(wait);

                    if (e.end_id == -1)
                    {
                        busRam.MoveDown();
                    }
                    else
                    {
                        cacheBus[e.end_id].MoveUp();

                    }

                }

                 // Wait for 1 second before the next iteration
                yield return new WaitForSeconds(wait);
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
public class Transaction
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
    public List<Transaction> transactions;
}