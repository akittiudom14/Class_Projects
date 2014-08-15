Twitter Trends
=

In this project, a partner and I created a geographic visualization of twitter data across the USA. We used dictionaries, lists, and data abstraction techniques to created a modular program that displays the sentiments of tweets about a certain word. This was done by:

1. Collecting public Twitter posts (tweets) that have been tagged with geographic locations and filtering for those that contain the query term. We did so by extracting the text, time, latitude, and longitude values from the abstract data type. 
2. Assigning a sentiment (positive or negative) to each tweet, based on all of the words it contains. Sentements are pulled from a pre-made dictionary of words and their sentiment values.
3. Aggregating tweets by the state with the closest geographic center. States are treated as polygons, for which the centroid is computed. 
4. Coloring each state on a spectrum according to the aggregate sentiment of its tweets. Red means positive sentiment; blue means negative. An example geographic visualization for the query word, "Texas", is shown in the texas.png file. 

The project was implemented in trends.py with the the other files as utility files. 
- geo.py: Geographic positions, 2-D projection equations, and geographic distance functions.
- maps.py: Functions for drawing maps.
- data.py: Functions for loading Twitter data from files.
- graphics.py: A simple Python graphics library.
- ucb.py: Utility functions for the course

note: The project relies on twitter data that is not included here

Acknowledgements: Aditi Muralidharan developed this project with John DeNero. Hamilton Nguyen extended it.

- Partner: Song King
