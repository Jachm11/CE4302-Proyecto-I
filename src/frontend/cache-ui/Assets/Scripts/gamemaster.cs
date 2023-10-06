using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Newtonsoft.Json;
using TMPro;
using System.Net.Sockets;
using Unity.VisualScripting;
using Unity.Collections;
using UnityEngine.UI;
using System;
using System.Text;
using UnityEditor.PackageManager;
using System.IO;

public class Gamemaster : MonoBehaviour
{
    public Cpu[] cpus = new Cpu[3];
    public Arrows[] cpuCache = new Arrows[3];
    public Cache[] caches = new Cache[3];
    public Arrows[] cacheBus = new Arrows[3];
    public Bus bus;
    public Arrows busRam;
    public Ram ram;
    public bool run;
    public TextMeshProUGUI log;
    public Button runButton;
    public Button stepButton;
    public Button resetButton;
    public Button protocolButton;
    public TextMeshProUGUI protocolButtonText;
    public Button cpu0Button;
    public Button cpu1Button;
    public Button cpu2Button;
    public TextMeshProUGUI connectButtonText;

    private Execution execution;
    private int[] regs = new int[3];
    private string protocol;
    private int transactionIndex = 0;
    private bool done = false;
    private float wait = 1.0f;

    private TcpClient client;
    private NetworkStream stream;
    private const string serverIP = "127.0.0.1";
    private const int serverPort = 4000;
    private string jsonData;


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

        GameObject[] cacheObjects = GameObject.FindGameObjectsWithTag("CACHE");

        for (int i = 0; i < cacheObjects.Length; i++)
        {
            Cache cacheComponent = cacheObjects[i].GetComponent<Cache>();
            char lastChar = cacheObjects[i].name[^1];
            int index = int.Parse(lastChar.ToString());
            caches[index] = cacheComponent;
        }

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
            regs[i] = (i+1)*100;
        }

        bus = GameObject.FindGameObjectWithTag("BUS").GetComponent<Bus>();

        busRam = GameObject.FindGameObjectWithTag("BUS_RAM").GetComponent<Arrows>();

        ram = GameObject.FindGameObjectWithTag("RAM").GetComponent<Ram>();

        cpu0Button.interactable = false;
        cpu1Button.interactable = false;
        cpu2Button.interactable = false;
        runButton.interactable = false;
        stepButton.interactable = false;
        resetButton.interactable = false;
        protocol = "MESI";

    }

    public void connect()
    {
        try
        {

            ConnectToServer();

            SendJsonData(protocol);

            execution = JsonConvert.DeserializeObject<Execution>(jsonData);

            log.text += "STARTING EMULATION WITH PROCOTOL " + protocol + "";

            cpu0Button.interactable = true;
            cpu1Button.interactable = true;
            cpu2Button.interactable = true;
            runButton.interactable = true;
            stepButton.interactable = true;
            resetButton.interactable = true;

        
            connectButtonText.text = "Request new instructions";


        }
        catch (System.Exception)
        {
            log.text += "\nCONNECTION ERROR: Server is down "+"\n";
            throw;
        }
        
    }

    public void Step()
    {

        runButton.interactable = false;
        protocolButton.interactable = false;
        StartCoroutine(ProcessTransaction());
        transactionIndex++;

    }

    public void Run()
    {

        stepButton.interactable = false;
        runButton.interactable = false;
        protocolButton.interactable = false;
        wait = 2.0f;
        StartCoroutine(ProcessTransactions());

    }

    public void Reset()
    {
        StopAllCoroutines();
        runButton.interactable = true;
        stepButton.interactable = true;
        protocolButton.interactable = true;
        transactionIndex = 0;
        done = false;
        log.text = "";

        for (int i = 0; i < 3; i++ )
        {
            cpus[i].ChangeRegister("R0 : " + (i+1)*100);
            cpus[i].ChangeInstruction("");
            
            for(int j = 0; j < 4; j++)
            {
                string newText = "0x" + (j).ToString("X") + " : 00 : I";
                caches[i].ChangeLine(newText,j);
            }
        }
        for(int i = 0; i < 16; i++)
            {
                string newText = "0x" + (i).ToString("X") + " : " + (i+1)*1000;
                ram.ChangeLine(newText,i);
            }
    }

    public void ChangeProtocol()
    {

        if (protocol == "MESI")
        {
            protocol = "MOESI";
            protocolButtonText.text = "MOESI"; 

        }
        else
        {
            protocol = "MESI";
            protocolButtonText.text = "MESI"; 
        }

    }

    public void ShowInstructions(int cpuId)
    {

        log.text += "\nInstructions on core " + cpuId + "\n\n";
        foreach (Core core in execution.cores)
        {
            if (core.id == cpuId){
                foreach (Instruction instruction in core.instructions)
                {
                    if (instruction.type == "INC")
                    {
                        log.text += "INC R0\n";
                    }
                    else
                    {
                        log.text += instruction.type + " 0x" +instruction.mem_addr.ToString("X") + "\n";
                    }
                }
            }
        }
    }

    private void ConnectToServer()
    {
        try
        {
            client = new TcpClient(serverIP, serverPort);
            stream = client.GetStream();
            Debug.Log("Connected to server");
        }
        catch (Exception e)
        {
            Debug.LogError("Error connecting to server: " + e.Message);
            throw new Exception("Error connecting to server: " + e.Message);
        }
    }

    public void SendJsonData(string message)
    {
        if (stream == null)
        {
            Debug.LogError("Not connected to the server.");
            throw new Exception("Not connected to the server");
        }

        try
        {
            Request data = new Request
            {
                command = "start",
                protocol = message
            };

            string json = JsonConvert.SerializeObject(data);
            byte[] jsonRequest = Encoding.UTF8.GetBytes(json);

            stream.Write(jsonRequest, 0, jsonRequest.Length);
            Debug.Log("Sent JSON data to server: " + json);

            // Initialize a MemoryStream to store the received data
            using (MemoryStream memoryStream = new MemoryStream())
            {
                // Create a temporary buffer for reading data
                byte[] buffer = new byte[1024];
                int bytesRead;

                // Read data from the stream until the server closes the connection
                while ((bytesRead = stream.Read(buffer, 0, buffer.Length)) > 0)
                {
                    // Write the read data to the MemoryStream
                    memoryStream.Write(buffer, 0, bytesRead);
                }

                // Convert the received data to a string
                string response = Encoding.UTF8.GetString(memoryStream.ToArray());

                Debug.Log("Received response from server: " + response);

                jsonData = response;
            }

            Disconnect();
        }
        catch (Exception e)
        {
            Debug.LogError("Error sending/receiving JSON data: " + e.Message);
        }
    }


    private void Disconnect()
    {
        if (client != null)
        {
            client.Close();
            Debug.Log("Socket closed.");
        }
    }

    IEnumerator ProcessTransactions()
    {
        while (!done)
        {
            yield return StartCoroutine(ProcessTransaction());
            transactionIndex++;
        }
    }

    IEnumerator ProcessTransaction()
    {

        if (transactionIndex < execution.transactions.Count)
        {
            Transaction transaction = execution.transactions[transactionIndex];

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
                if (transaction.type == "READ")
                {
                    yield return new WaitForSeconds(wait/2);
                    cacheBus[transaction.core].MoveDown();
                }   
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
                        log.text += "       Cache " + e.element_id + " was written " + e.data + " on line " + (e.tag % 4).ToString("X") + "\n";
                        log.text += "       Line chaged to state " + state + "\n";
                        caches[e.element_id].ChangeLine(newText, e.tag % 4);
                    }

                }

                else if (e.event_type == "move")
                {
                    
                    if (e.start_id == -1){
                        busRam.MoveUp();
                        
                    }
                    else if(e.start_id != e.end_id)
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
        else
        {
            // PRINT REPORT
            log.text +=  "\n" + "__________________________________" + "\n";
            log.text +=         "PROGRAM FINISHED: PRINTING RESULTS" + "\n";
            log.text +=         "__________________________________" + "\n" + "\n";
            log.text +=  "Number of cores: " + execution.report.cores + "\n";
            log.text +=  "Instructions per core: " + execution.report.instructions_per_core + "\n";
            log.text +=  "Blocks per cache: " + execution.report.blocks_per_cache + "\n";
            log.text +=  "RAM size: " + execution.report.blocks_per_ram + "\n";
            log.text +=  "Increments done: " + execution.report.increments_counter + "\n";
            log.text +=  "Total read requests: " + execution.report.read_requests_count + "\n";
            log.text +=  "Total write requests: " + execution.report.write_requests_count + "\n";
            log.text +=  "Total RAM reads: " + execution.report.ram_reads_counter + "\n";
            log.text +=  "Total write responses: " + execution.report.write_responses + "\n";
            log.text +=  "Total read responses: " + execution.report.read_responses + "\n";
            log.text +=  "Total cache invalidates: " + execution.report.invalidates_counter + "\n";

            done = true;
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
public class Instruction
{
    public int mem_addr;
    public string type;
}

[System.Serializable]
public class Core
{
    public int id;
    public List<Instruction> instructions;
}

[System.Serializable]
public class Report
{
    public int cores;
    public int instructions_per_core;
    public int blocks_per_cache;
    public int blocks_per_ram;
    public int increments_counter;
    public int read_requests_count;
    public int write_requests_count;
    public int ram_reads_counter;
    public int write_responses;
    public int read_responses;
    public int invalidates_counter;
}

[System.Serializable]
public class Execution
{
    public List<Core> cores;
    public Report report;
    public List<Transaction> transactions;
}

[System.Serializable]
public class Request
{
    public string command;
    public string protocol;
}