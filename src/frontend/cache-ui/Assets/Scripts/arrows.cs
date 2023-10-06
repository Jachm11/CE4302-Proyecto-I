using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Arrows : MonoBehaviour
{
    public UnityEngine.Rendering.Universal.Light2D light2D;

    private Vector2 originalPosition;
    private Quaternion originalRotation;

    void Start()
    {
        
        // Check if the reference to the Light2D component is not null
        if (light2D != null)
        {
            // Set the intensity to 0
            light2D.intensity = 0f;
        }
        else
        {
            // Log an error message if the reference is null
            Debug.LogError("Light2D component is not assigned.");
        }

        // Store the original position of the object
        originalPosition = light2D.transform.localPosition;
        originalRotation = transform.rotation;
    }

    public void MoveDown()
    {
        // Reset the object to the original position
        light2D.transform.localPosition = originalPosition;
        transform.rotation = originalRotation;

        light2D.intensity = 100f;

        Vector2 targetPosition = originalPosition - new Vector2(-50f, 0f);

        // Use LeanTween for moving the light
        light2D.transform.LeanMoveLocal(targetPosition, 1f)
            .setEaseOutCirc()
            .setOnComplete(FadeOutLight);
    }


     public void MoveUp()
    {
        // Reset the object to the orginal position
        light2D.transform.localPosition = originalPosition;
        transform.rotation = originalRotation;

        transform.rotation = Quaternion.Euler(0f, 0f, 90f);

        Vector2 targetPosition = originalPosition - new Vector2(-50f, 0f);


        // Reset the intensity to 0
        light2D.intensity = 100f;


        // Use LeanTween for moving the light
        light2D.transform.LeanMoveLocal(targetPosition, 1f)
            .setEaseOutCirc()
            .setOnComplete(FadeOutLight);
    }


    //     _________________
    //____/Private functions


    private void FadeOutLight()
    {
        // Start a coroutine to gradually reduce the light intensity
        StartCoroutine(FadeLightIntensity());
    }

    private IEnumerator FadeLightIntensity()
    {
        float duration = 0.5f; // Adjust the duration as needed
        float startIntensity = light2D.intensity;
        float targetIntensity = 0f;
        float elapsedTime = 0f;

        while (elapsedTime < duration)
        {
            // Calculate the new intensity using Lerp
            float newIntensity = Mathf.Lerp(startIntensity, targetIntensity, elapsedTime / duration);

            // Set the light intensity
            light2D.intensity = newIntensity;

            elapsedTime += Time.deltaTime;
            yield return null;
        }

        // Ensure the intensity is exactly the target intensity
        light2D.intensity = targetIntensity;
    }
}