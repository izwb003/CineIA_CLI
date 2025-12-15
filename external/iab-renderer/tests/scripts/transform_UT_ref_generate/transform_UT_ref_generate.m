% Generates unit test data

ming_row_vector = [1, 2, 3];
ming_column_vector = [1; 2; 3];

% Use theatrical 7.1 only for IAB. All corner, speaker, etc coordinates are 
% in reference to theatrical 7.1

% Room Corners, 8x rows. 
% azimuth, elevation
global roomCorners = [
    -37.5,  0;
    37.5,   0;
    -142.5, 0;
    142.5,  0;
    -37.5,  25.97;
    37.5,   25.97;
    -142.5, 25.97;
    142.5,  25.97
];

% Room Speaker, 16x rows. 
% azimuth, elevation
global roomSpeakers = [
    -30,     0;
    30,      0;
    -150,    0;
    150,     0;
    -45,     0;
    45,      0;
    -135,    0;
    135,     0;
    -24.79,  35.99;
    24.79,   35.99;
    -155.21, 35.99;
    155.21,  35.99;
    -24.79,  35.99;
    24.79,   35.99;
    -155.21, 35.99;
    155.21,  35.99
];

% Room Speaker, mid-plane 16x rows. 
% azimuth, elevation
global roomSpeakers_mid = [
    -30,     0;
    30,      0;
    -150,    0;
    150,     0;
    -45,     0;
    45,      0;
    -135,    0;
    135,     0;
    -30,     21.0;
    30,      21.0;
    -150,    21.0;
    150,     21.0;
    -45,     22.2;
    45,      22.2;
    -135,    22.2;
    135,     22.2
];

% Calculate room corner coordinates, in Cartesian.
global roomCornersCoordinates = zeros(rows(roomCorners), 3);

for i = 1:rows(roomCorners)
  
  if (roomCorners(i, 1) >= -90 && roomCorners(i, 1) <= 90)
    y = 1;
    x = y * tan(roomCorners(i, 1) * pi / 180);
  else
    y = -1;
    x = y * tan(roomCorners(i, 1) * pi / 180);
  endif
   
  h = sqrt(x^2 + y^2);
  z = h * tan(roomCorners(i, 2) * pi / 180);
  
  roomCornersCoordinates(i, 1) = x;
  roomCornersCoordinates(i, 2) = y;
  roomCornersCoordinates(i, 3) = z;

endfor

% Calculate room speaker coordinates, in Cartesian.
global roomspeakersCoordinates = zeros(rows(roomSpeakers), 3);

% Variables for height speakers tweak
plane_3point = zeros(3);
line = zeros(3, 1);

for i = 1:rows(roomSpeakers)
  
  if (roomSpeakers(i, 1) >= -90 && roomSpeakers(i, 1) <= 90)
    y = 1;
    x = y * tan(roomSpeakers(i, 1) * pi / 180);
  else
    y = -1;
    x = y * tan(roomSpeakers(i, 1) * pi / 180);
  endif
   
  h = sqrt(x^2 + y^2);
  z = h * tan(roomSpeakers(i, 2) * pi / 180);
  
  % for height speakers only
  if (roomSpeakers(i, 2) > 0)
    % point
    plane_3point(1, 1) = x;
    plane_3point(2, 1) = y;
    plane_3point(3, 1) = z;

    % left top corner - right top corner
    plane_3point(1, 2) = roomCornersCoordinates(5, 1) - roomCornersCoordinates(6, 1);
    plane_3point(2, 2) = roomCornersCoordinates(5, 2) - roomCornersCoordinates(6, 2);
    plane_3point(3, 2) = roomCornersCoordinates(5, 3) - roomCornersCoordinates(6, 3);
    
    % left rear corner - left top corner
    plane_3point(1, 3) = roomCornersCoordinates(7, 1) - roomCornersCoordinates(5, 1);
    plane_3point(2, 3) = roomCornersCoordinates(7, 2) - roomCornersCoordinates(5, 2);
    plane_3point(3, 3) = roomCornersCoordinates(7, 3) - roomCornersCoordinates(5, 3);
    
    % Invert
    invPlane = inv(plane_3point);
    
    % Line
    line(1, 1) = x - roomCornersCoordinates(5, 1);
    line(2, 1) = y - roomCornersCoordinates(5, 2);
    line(3, 1) = z - roomCornersCoordinates(5, 3);
    
    int_point = invPlane * line;
    modifier = 1 - int_point(1);
    
    x = x * modifier;
    y = y * modifier;
    z = z * modifier;
  endif
  
  roomspeakersCoordinates(i, 1) = x;
  roomspeakersCoordinates(i, 2) = y;
  roomspeakersCoordinates(i, 3) = z;

endfor

% Calculate mid plane room speaker coordinates, in Cartesian.
global roomspeakersMidCoordinates = zeros(rows(roomSpeakers_mid), 3);

for i = 1:rows(roomSpeakers_mid)
  
  if (roomSpeakers_mid(i, 1) >= -90 && roomSpeakers_mid(i, 1) <= 90)
    y = 1;
    x = y * tan(roomSpeakers_mid(i, 1) * pi / 180);
  else
    y = -1;
    x = y * tan(roomSpeakers_mid(i, 1) * pi / 180);
  endif
   
  h = sqrt(x^2 + y^2);
  z = h * tan(roomSpeakers_mid(i, 2) * pi / 180);
  
  roomspeakersMidCoordinates(i, 1) = x;
  roomspeakersMidCoordinates(i, 2) = y;
  roomspeakersMidCoordinates(i, 3) = z;

  % Original comment: "roomSpeakers coordinates will represent upper half of room"
  % From index 9, out of 16 speakers
  if (i >= 9)
    roomspeakersCoordinates(i - 8, 1) = x;
    roomspeakersCoordinates(i - 8, 2) = y;
    roomspeakersCoordinates(i - 8, 3) = z;    
  endif

endfor

% Y listener position in middle
global roomListenerLocation = 1;

% PT-based bounds, used by algorithm
global yFrontSideSpeaker = 75;
global yRearSideSpeaker = -75;

% ********************************************************************
% Function for transform a position
% Range for each dimension: [-1 +1], for both input and output
%
function [xout, yout, zout] = shoebox_transform (xin, yin, zin)

  global roomCorners;
  global roomSpeakers;
  global roomSpeakers_mid;
  global roomCornersCoordinates;
  global roomspeakersMidCoordinates;
  global roomspeakersCoordinates;
  global roomListenerLocation;
  global yFrontSideSpeaker;
  global yRearSideSpeaker;

  % Create scaledSpeakerCoordinates
  scaledSpeakerCoordinates = zeros(rows(roomSpeakers), 3);
  
  % rows 1:8
  for i = 1:8
    
    if (zin <= 0.5)
      scaledSpeakerCoordinates(i, 1) = roomspeakersMidCoordinates(i, 1);
      scaledSpeakerCoordinates(i, 2) = roomspeakersMidCoordinates(i, 2);
      scaledSpeakerCoordinates(i, 3) = roomspeakersMidCoordinates(i, 3);
    else
      scaledSpeakerCoordinates(i, 1) = roomspeakersCoordinates(i, 1);
      scaledSpeakerCoordinates(i, 2) = roomspeakersCoordinates(i, 2);
      scaledSpeakerCoordinates(i, 3) = roomspeakersCoordinates(i, 3);
    endif
  
  endfor

  % rows 9:16
  for i = 9:16
    
    if (zin <= 0.5)
      scaledSpeakerCoordinates(i, 1) = roomspeakersMidCoordinates(i, 1) + ((0.5 - zin) / 0.5) * (roomspeakersMidCoordinates(i - 8, 1) - roomspeakersMidCoordinates(i, 1));
      scaledSpeakerCoordinates(i, 2) = roomspeakersMidCoordinates(i, 2) + ((0.5 - zin) / 0.5) * (roomspeakersMidCoordinates(i - 8, 2) - roomspeakersMidCoordinates(i, 2));
      scaledSpeakerCoordinates(i, 3) = roomspeakersMidCoordinates(i, 3) * (zin / 0.5);     
    else
      scaledSpeakerCoordinates(i, 1) = roomspeakersCoordinates(i, 1) + ((1.0 - zin) / 0.5) * (roomspeakersCoordinates(i - 8, 1) - roomspeakersCoordinates(i, 1));
      scaledSpeakerCoordinates(i, 2) = roomspeakersCoordinates(i, 2) + ((1.0 - zin) / 0.5) * (roomspeakersCoordinates(i - 8, 2) - roomspeakersCoordinates(i, 2));
      scaledSpeakerCoordinates(i, 3) = roomspeakersCoordinates(i - 8, 3) + (roomspeakersCoordinates(i, 3) - roomspeakersCoordinates(i - 8, 3)) * ((zin - 0.5) / 0.5);
    endif
    
  endfor
  
  % Original comments: "Scale x and y to plane defined by roomSpeakerCoordinates"
  slope = 0;
  slope_infinity = 0;
  xabs = abs(xin);
  
  x1 = 0;
  y1 = 0;
  z1 = 0;
  
  x2 = 0;
  y2 = 0;
  % z1 = 0;

  xi = 0;
  yi = 0;
  zi = 0;
  
  xs1 = 0;
  ys1 = 0;
  xs2 = 0;
  ys2 = 0;
  
  yside = 0;
  
  kEPSILON = 9.53674316e-07;
  
  elevation = 0;
  azimuth = 0;
  radius = 0;
  
  r = 0;
  rObj = 0;
  
  isCeiling = 0;
  
  if (yin >= 0)

    % Determine if object is in front of side speaker wedge (line from side speaker to listener)
    if (xabs > kEPSILON)
      slope = yin / xabs;
    else
      slope_infinity = 1;
    endif
    
    % Determine slope of side speaker to listener and scale to corner as zin approaches ceiling
    ySide = yFrontSideSpeaker / 100.0;
    
    if (zin > 0.5)
      ySide = ySide + (1.0 - ySide) * ((zin - 0.5) * 2);
    endif

    % c_roomRightFrontSideSpeakerTop
    ys1 = scaledSpeakerCoordinates(14, 2);
    xs1 = scaledSpeakerCoordinates(14, 1);
    
    % c_roomRightFrontSpeakerTop
    ys2 = scaledSpeakerCoordinates(10, 2);
    xs2 = scaledSpeakerCoordinates(10, 1);
    
    % Set listener z coordinate
    if ((slope >= 1.0) || slope_infinity == 1)
      y1 = yin * ys2;
      x1 = xin * xs2;
      z1 = scaledSpeakerCoordinates(10, 3);
    elseif (slope >= ySide)
      % Object is in front of line from side speaker to listener
      y1 = xabs * ys2;
      
      if (1.0 - ySide != 0.0)
        x1 = xin * xs2 + (xin * (1 - slope)) * ((xs1 - xs2) / (1.0 - ySide));
            
        z1 = ((slope - ySide) / (1.0 - ySide)) * scaledSpeakerCoordinates(10, 3) + (1 - ((slope - ySide) / (1.0 - ySide))) * scaledSpeakerCoordinates(14, 3);
      else
        x1 = x * xs1;
        z1 = scaledSpeakerCoordinates(10, 3);
      endif
    else
      % Object is behind line from side speaker to listener (but in front of listener)
      y1 = (yin / ySide) * ys1;
      x1 = xin * xs1;
      z1 = scaledSpeakerCoordinates(14, 3);
    endif
    
  % if (yin >= 0)      
  else

    % Determine if object is behind side speaker wedge (line from side speaker to listener)
    if (xabs > kEPSILON)
      slope = -yin / xabs;
    else
      slope_infinity = 1;
    endif
    
    % Determine slope of side speaker to listener and scale to corner as z approaches ceiling
    ySide = -yRearSideSpeaker / 100.0;
    
    if (zin > 0.5)
      ySide = ySide + (1.0 - ySide) * ((zin - 0.5) * 2);
    endif
    
    % c_roomRightRearSideSpeakerTop
    ys1 = -scaledSpeakerCoordinates(16, 2);
    xs1 = scaledSpeakerCoordinates(16, 1);
    
    % c_roomRightRearSpeakerTop
    ys2 = -scaledSpeakerCoordinates(12, 2);
    xs2 = scaledSpeakerCoordinates(12, 1);
    
    if ((slope >= 1.0) || slope_infinity == 1)
      y1 = yin * ys2;
      x1 = xin * xs2;
      z1 = scaledSpeakerCoordinates(12, 3);
    elseif (slope >= ySide)
      % Object is behind the line from side speaker to listener
      y1 = xabs * -ys2;
      if (1.0 - ySide != 0.0)
        x1 = xin * xs2 + (xin * (1 - slope)) * ((xs1 - xs2) / (1.0 - ySide));
        z1 = ((slope - ySide) / (1.0 - ySide)) * scaledSpeakerCoordinates(12, 3) + (1 - ((slope - ySide) / (1.0 - ySide))) * scaledSpeakerCoordinates(16, 3);
      else
        x1 = x * xs1;
        z1 = scaledSpeakerCoordinates(16, 3);
      endif
    else
      % Object is in front of line from side speaker to listener (but behind the listener)
      y1 = (yin / ySide) * ys1;
      x1 = xin * xs1;
      z1 = scaledSpeakerCoordinates(16, 3);
    endif
  
  % if (yin >= 0)    
  endif

  r = sqrt(x1 ^ 2 + y1 ^ 2 + z1 ^ 2);
  if (r != 0.0)
    elevation = asin(z1 / r);
    azimuth = atan2(x1, y1);
      
    if (elevation < 0.0)
      elevation = 0.0;
    endif
  else
    elevation = 0.0;
    azimuth = 0.0;
  endif
  
  % Original comment "Compute radius by first determining the intersection from the 
  % listener through the object to the edge of the box"
  % Use 1st quadrant for all calculations. 
  x2 = xin;
  if (x2 < 0)
    x2 = -x2;
  endif
  
  y2 = yin;
  if (y2 < 0)
    y2 = -y2;
  endif
  
  if (zin > 0)
    % Check if intersection of listener to object vector with plane of ceiling is within ceiling bounds
    if (x2 / zin < 1.0 && y2 / zin < 1.0)
      isCeiling = 1;
    endif
  endif
  
  if (isCeiling != 0)
    % Intersect with ceiling
    xi = x2 / zin;
    yi = y2 / zin;
    zi = 1.0;
  else
    if (x2 > y2)
      % Intersect with right wall
      if (x2 != 0)
        xi = 1.0;
        yi = y2 / x2;
        zi = zin / x2;
      else
        % Intersect at right mid-point
        xi = 1.0;
        yi = 0.0;
        zi = 0.0;
      endif
    else
      % Intersect with front wall
      if (y2 != 0)
        xi = x2 / y2;
        yi = 1.0;
        zi = zin / y2;
      else
        % Intersect at front mid-point
        xi = 0.0;
        yi = 1.0;
        zi = 0.0;
      endif
    endif
  endif
  
  % Compute length of wall intersection and ratio with length to object
  r = sqrt(xi ^ 2 + yi ^ 2 + zi ^ 2);
  rObj = sqrt(x2 ^ 2 + y2 ^ 2 + zin ^ 2);
  if (r != 0.0)
    radius = rObj / r;
  else
    radius = 0.0;
  endif
  
  % converting to Cartesian
        
  xout = radius * cos(elevation) * sin(azimuth);
  yout = radius * cos(elevation) * cos(azimuth);
  zout = radius * sin(elevation);

endfunction

% ***********************************************************
% Main loop

fp = fopen("IABToMDAPositionTransformTests.h", "w");

fprintf(fp, "\r\n\
/*\r\n\
 * IABToMDAPositionTransformTests\r\n\
 * \r\n\
 * {iab_x, iab_y, iab_z, mda_x, mda_y, mda_z}\r\n\
 *\r\n\
 * Generated using algorithms as implemented in shoeboxTransform() of MDACreator. Jan 15, 2019.\r\n\
 */\r\n\
");

fprintf(fp, "float IABToMDAPositionTransformTests[][] = { \r\n");

% Looping through each of IAB Cartesian (x, y, z) components, all with range [0, +1].
% Step size = 0.125
% x = i
% y = j
% z = k
%
for i = 0:0.125:1

  for j = 0:0.125:1
  
    for k = 0:0.125:1
    
      % Excluding RLP position (origin)
      if (i == 0.5 && j == 0.5 && k == 0)
        continue;
      endif
      
      % Translate to center-origin coordinate
      xCen = 2.0 * i - 1;
      yCen = 1 - 2.0 * j;
      zCen = k;
      
      % Call function
      [xmda, ymda, zmda] = shoebox_transform (xCen, yCen, zCen);
      
      % Write out input & out test data
      fprintf(fp, "   {%f, %f, %f, %f, %f, %f}", i, j, k, xmda, ymda, zmda); 
      
      if (! (i == 1 && j == 1 && k == 1))
        fprintf(fp, ",");
      endif
      
      fprintf(fp, "\r\n");
    
    endfor
  
  endfor
  
endfor

fprintf(fp, "   };\r\n");

fclose(fp);

