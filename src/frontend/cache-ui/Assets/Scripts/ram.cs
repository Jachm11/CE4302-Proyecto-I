using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class Ram : MonoBehaviour
{
    public UnityEngine.Rendering.Universal.Light2D light2D;
    public TextMeshProUGUI[] textMeshProObjects = new TextMeshProUGUI[16];
    public int cacheLine;
    private float originalIntensity;

    void Start()
    {
        // Check if the reference to the Light2D component is not null
        if (light2D != null)
        {
            // Store the original light intensity
            originalIntensity = light2D.intensity;

            // Set the intensity to 0 initially
            light2D.intensity = 0f;
        }
        else
        {
            // Log an error message if the reference is null
            Debug.LogError("Light2D component is not assigned.");
        }
    }

    // Public function to change the text of a TextMeshPro object based on ID
    public void ChangeLine(string newText)
    {
        int id = cacheLine;

        // Check if the provided ID is within a valid range
        if (id >= 0 && id < textMeshProObjects.Length)
        {
            // Update the text of the TextMeshPro object at the specified ID
            textMeshProObjects[id].text = newText;

            // Start a coroutine to gradually change the light intensity
            StartCoroutine(ChangeLightIntensity());
        }
        else
        {
            // Log an error message if the provided ID is out of range
            Debug.LogError("Invalid ID. ID should be in the range of 0 to " + (textMeshProObjects.Length - 1));
        }
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