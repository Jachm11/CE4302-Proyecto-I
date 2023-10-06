using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bus : MonoBehaviour
{

    public UnityEngine.Rendering.Universal.Light2D light2D;

    private Vector2 originalPosition;

    // Start is called before the first frame update
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

        originalPosition = light2D.transform.localPosition;
        
    }


    public void Move(int idOrg, int idDest)
    {

        int direction = 1;
        int multiplier = 1;
        // Reset the object to the orginal position
        switch (idOrg)
        {
            
            case -1:

                light2D.transform.localPosition = originalPosition;
                light2D.color = Color.white;
                if (idDest == 0)
                {
                    direction = 1;
                }
                if (idDest == 2)
                {
                    direction = -1;
                }
                else
                {
                    direction = 0;
                }
                break;
            
            case 0:

                light2D.transform.localPosition = originalPosition;
                light2D.color = Color.green;
                direction = -1;
                break;
            
            case 1:

                light2D.transform.localPosition = originalPosition + new Vector2(700f,0f);
                light2D.color = Color.cyan;
                if (idDest > idOrg)
                {
                    direction = -1;
                }
                else
                {
                    direction = 1;
                }
                break;

            case 2:

                light2D.transform.localPosition = originalPosition + new Vector2(1400f,0f);
                light2D.color = Color.magenta;
                direction = 1;
                break;

            default:

                light2D.transform.localPosition = originalPosition;
                break;

        }

        if (idDest == idOrg){
            direction = 0;
            light2D.color = Color.red;
        }
        else if (idDest != 1 && idDest != -1)
        {
            multiplier = 2;
        }
        else if (idOrg == 1 && idDest == -1){
            direction = 0;
        }
        else
        {
            multiplier = 1;
        }

        Vector2 targetPosition = light2D.transform.localPosition - new Vector3(direction * 700f * multiplier, 0f, 0f);

        // Reset the intensity to 0
        light2D.intensity = 20f;

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
