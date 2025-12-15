
% Generates unit test data
% Reference values are calculated from the formula given here

0;
fp = fopen("INTERIOROBJECTTONORMMDAOBJECTSCONVERSIONTESTS.h", "w");

fprintf(fp, "\r\n\
/*\r\n\
 * INTERIOROBJECTTONORMMDAOBJECTSCONVERSIONTESTS\r\n\
 * \r\n\
 * {radius, azimuth, elevation, azimuth_left, elevation_left, azimuth_right, elevation_right, azimuth_center, elevation_center, normGain_left, normGain_right, normGain_center}\r\n\
 *\r\n\
 * Generated using the interior object to normalised objects conversion algorithm  r\n\
 */\r\n\
");

fprintf(fp, "float INTERIOROBJECTTONORMMDAOBJECTSCONVERSIONTESTS[953][9] = { \r\n");


IAB_GAIN_TABLE_SIZE = 201;
gainTableIndex = 1;
eMaxNumNormObjects  = 3;
c_inwardPanObjectZeroGain = 75;
m_GainTable=zeros(IAB_GAIN_TABLE_SIZE)

for r = 0.0:0.1:1.0
  
  for azimuth_deg = 0:22.5:360
  
    for elevation_deg = 0:15:90
      
      if (r >= 1.0)
        
      fprintf(fp, "   {%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f},", r, azimuth, elevation, azimuth, elevation, azimuth, elevation, azimuth, elevation, 0.0, 0.0, 1.0); 
      fprintf(fp, "\r\n");
      else
       
      azimuth = 0.0174532925 * azimuth_deg;
      elevation = 0.0174532925 * elevation_deg;
      iabKPI = 3.141592653589793;
      kepsilon = 0.000000953674316;

      height = r * sin(elevation);
      rObj = r * cos(elevation);
      y1 = rObj * cos (azimuth);
      x1 = rObj * sin (azimuth);
      rcircle = sqrt (1 - height * height );
      
        if (x1 > rcircle)
            x1 = rcircle;
        elseif (x1 < -rcircle)
            x1 = -rcircle;
        endif
        
        if (y1 > rcircle)
            y1 = rcircle;
        elseif (y1 < -rcircle)
            y1 = -rcircle;
        endif
      
      xright = sqrt ( rcircle * rcircle - y1 * y1);
      
      if (y1 == 0.0)
        Azimuth_right = 0.0174532925 * 90;
      else
        Azimuth_right = atan2 ( xright , y1);
      endif
      Azimuth_left = - Azimuth_right;
      Azimuth_center = azimuth;
      
      if (rcircle == 0.0)
        elevation_left = 0.0174532925 * 90;
      else
        elevation_left = atan2 (height , rcircle);
      endif
      
       if (elevation_left > 3.141592653589793 / 2.0)
            elevation_left = 3.141592653589793 / 2.0;
       endif
       
        %Gain calculations
      

      
      % Init gain table for mapping 2d panner x coordinate to linear gain
        % ProTools uses a sin/cos profile
        for i = 1 : 1 : IAB_GAIN_TABLE_SIZE
        
            m_GainTable(i) = sin(((i-1) / IAB_GAIN_TABLE_SIZE) * iabKPI / 2.0);
        
        endfor
        
        
         %Right gain is distance from object x coordinate to right intersection, scaled by length of horizontal line
        if (xright > kepsilon)
            value = ((xright + x1) * IAB_GAIN_TABLE_SIZE ) / (2.0 * xright);
            gainTableIndex = floor(value);
        else
            gainTableIndex = floor(IAB_GAIN_TABLE_SIZE / 2);
        endif
        
        if (gainTableIndex < 0)
            gainTableIndex = 0;
        elseif (gainTableIndex >= IAB_GAIN_TABLE_SIZE)
            gainTableIndex = IAB_GAIN_TABLE_SIZE - 1;
        endif
        
        normGain_right = m_GainTable(gainTableIndex + 1);
        normGain_left = m_GainTable(IAB_GAIN_TABLE_SIZE - 1 - gainTableIndex + 1);
        
         % Normalize gains to 1.0
        gainSum = normGain_left + normGain_right;
        normGain_right *= 1.0 / gainSum;
        normGain_left*= 1.0 / gainSum;
        
        
         % Linearly scale gain based on radius of object and constant indicating where gain goes to zero
        normGain_center = ((100.0 * r) - c_inwardPanObjectZeroGain) / (100.0 - c_inwardPanObjectZeroGain);
        if (normGain_center < 0.0)
            normGain_center = 0.0;
        endif
        
            
        %  Noramlized room side angles
      Norm_side_front_left = -45.0;
      Norm_side_front_right = 45.0;
      Norm_side_rear_left = -135.0;
      Norm_side_rear_right = 135.0;
      
         % Normaized room side speakers
        nsfl = Norm_side_front_left * iabKPI / 180.0;
        nsfr = Norm_side_front_right * iabKPI / 180.0;
        nsrl = Norm_side_rear_left * iabKPI / 180.0;
        nsrr = Norm_side_rear_right * iabKPI / 180.0;
        
         % Restrict side objects to norm room side speaker locations
        if (Azimuth_right < nsfr)
        {
            normGain_right *= Azimuth_right / nsfr;
           normGain_left *= Azimuth_right / nsfr;
            
            Azimuth_right = nsfr;
            Azimuth_left = nsfl;
        } elseif (Azimuth_right > nsrr)
        {
           normGain_right *= (iabKPI - Azimuth_right) / (iabKPI - nsrr);
           normGain_left *= (iabKPI - Azimuth_right) / (iabKPI - nsrr);
            
            Azimuth_right = nsrr;
            Azimuth_left = nsrl;
        }
        endif
      
      elevation_right = elevation_left;
      elevation_center = elevation;
     
      
         % Recompute normalized gain with third object
        gainSum = normGain_left + normGain_right + normGain_center;
        normGain_left = normGain_left * 1.0 / gainSum;
        normGain_right = normGain_right * 1.0 / gainSum;
        normGain_center = normGain_center * 1.0 / gainSum;

         
      fprintf(fp, "   {%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f},", r, azimuth, elevation, Azimuth_left, elevation_left, Azimuth_right, elevation_right, Azimuth_center, elevation_center, normGain_left, normGain_right, normGain_center); 
      
      fprintf(fp, "\r\n");
      endif
    
    endfor
  
  endfor
  
endfor

fprintf(fp, "   };\r\n");

fclose(fp);