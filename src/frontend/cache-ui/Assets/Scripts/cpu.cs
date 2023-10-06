using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class Cpu : MonoBehaviour
{
    public UnityEngine.Rendering.Universal.Light2D light2D;
    public UnityEngine.Rendering.Universal.Light2D regLight2D;
    public TextMeshProUGUI register;
    public TextMeshProUGUI instruction;
    private float originalIntensity;
    private float regOriginalIntensity;

    void Start()
    {
        // Check if the reference to the Light2D component is not null
        if (light2D != null)
        {
            // Store the original light intensity
            originalIntensity = light2D.intensity;
            regOriginalIntensity = regLight2D.intensity;

            // Set the intensity to 0 initially
            light2D.intensity = 0.7f;
            regLight2D.intensity = 0f;
        }
        else
        {
            // Log an error message if the reference is null
            Debug.LogError("Light2D component is not assigned.");
        }

        instruction.text = " ";

    }

    public void ChangeInstruction(string newText)
    {
 
        // Update the text of the TextMeshPro object at the specified ID
        instruction.text = newText;

        // Start a coroutine to gradually change the light intensity
        StartCoroutine(ChangeLightIntensity());

    }

    public void ChangeRegister(string newText)
    {
 
        // Update the text of the TextMeshPro object at the specified ID
        register.text = newText;
        regLight2D.intensity = regOriginalIntensity;

        // Start a coroutine to gradually change the light intensity
        StartCoroutine(ChangeLightIntensity());

    }

    // Coroutine to change the light intensity
    private IEnumerator ChangeLightIntensity()
    {
        float elapsedTime = 0f;
        float intensityChangeDuration = 0.5f;
        float startIntensity = light2D.intensity;

        // Gradually increase the intensity to a high value
        while (elapsedTime < intensityChangeDuration)
        {
            light2D.intensity = Mathf.Lerp(0f, originalIntensity * 5f, elapsedTime / intensityChangeDuration);
            elapsedTime += Time.deltaTime;
            yield return null;
        }
            
        // Gradually decrease the intensity back to the original value
        elapsedTime = 0f;
        while (elapsedTime < intensityChangeDuration)
        {
            light2D.intensity = Mathf.Lerp(originalIntensity * 5f, startIntensity, elapsedTime / intensityChangeDuration);
            elapsedTime += Time.deltaTime;
            yield return null;
        }

        // Ensure that the intensity is set to the original value
        light2D.intensity = originalIntensity;
    }
}