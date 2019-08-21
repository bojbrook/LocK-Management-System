package com.cmpe123.lock_management_system.ucsclocks;

import android.net.Uri;
import android.nfc.Tag;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URI;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by bowenbrooks on 5/10/18.
 */

public class AppEngineFetchr {
    private static final String TAG = "AppEngineFetchr";

    public byte[] getURlByte(String urlSpec) throws IOException{
        URL url = new URL(urlSpec);
        HttpURLConnection connection = (HttpURLConnection)url.openConnection();

        try{
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            InputStream in = connection.getInputStream();

            if (connection.getResponseCode() != HttpURLConnection.HTTP_OK){
                throw new IOException(connection.getResponseMessage() +
                        ": with " +
                        urlSpec);
            }

            int bytesRead = 0;
            byte[] buffer = new byte[1024];
            while ((bytesRead = in.read(buffer)) > 0){
                out.write(buffer,0,bytesRead);
            }
            out.close();
            return out.toByteArray();
        }finally {
                connection.disconnect();
        }
    }

    public String getUrlString(String urlSpec) throws IOException{
        return new String(getURlByte(urlSpec));
    }

    /*Fetches the Room for a given cruzID*/
    public List<Room> fetchItems(String cruzID) {
        List<Room> Rooms = new ArrayList<>();
        try {
            String url = Uri.parse("https://lock-management-system.appspot.com/android/" + cruzID)
                    .buildUpon()
                    .appendQueryParameter("method","GET")
                    .appendQueryParameter("format","json")
                    .build().toString();
            String jsonString = getUrlString(url);
            Log.i(TAG,"Received JSON: " + jsonString);
            JSONArray jsonArray = new JSONArray(jsonString);
            parseItems(Rooms,jsonArray);
        }catch (IOException ioe){
            Log.e(TAG,"Failed to fetch item", ioe);
        } catch (JSONException e) {
            Log.e(TAG,"Failed to parse JSON",e);
        }

        return Rooms;
    }

    public void parseItems(List<Room> Rooms, JSONArray roomJsonArray)
            throws IOException, JSONException{


        for(int i = 0; i < roomJsonArray.length();  i++){
            JSONObject roomJSONObject = roomJsonArray.getJSONObject(i);

            Log.i(TAG, roomJSONObject.getString("Room"));
            Log.i(TAG,"" + roomJSONObject.getInt("Capacity"));

            Room item = new Room();
            item.setRoomNumber(roomJSONObject.getString("Room"));
            item.setCapacity(roomJSONObject.getInt("Capacity"));

            Rooms.add(item);
        }

    }

    /*Fetches the logs for the given user and room_number*/
    public List<RoomLog> fetchLogs(String cruzID,String roomNumber) {
        List<RoomLog> Logs = new ArrayList<>();
        try {
            String url = Uri.parse("https://lock-management-system.appspot.com/android/" + cruzID + "/" + roomNumber)
                    .buildUpon()
                    .appendQueryParameter("method","GET")
                    .appendQueryParameter("format","json")
                    .build().toString();
            String jsonString = getUrlString(url);
            Log.i(TAG,"Received JSON: " + jsonString);
            JSONArray jsonArray = new JSONArray(jsonString);
            parseLogs(Logs,jsonArray);
        }catch (IOException ioe){
            Log.e(TAG,"Failed to fetch item", ioe);
        } catch (JSONException e) {
            Log.e(TAG,"Failed to parse JSON",e);
        }

        return Logs;
    }

    public void parseLogs(List<RoomLog> logs, JSONArray roomJsonArray)
            throws IOException, JSONException{


        for(int i = 0; i < roomJsonArray.length();  i++){
            JSONObject roomJSONObject = roomJsonArray.getJSONObject(i);

            Log.i(TAG, roomJSONObject.getString("Enter_Time"));
            Log.i(TAG, roomJSONObject.getString("Exit_Time"));
            Log.i(TAG,"" + roomJSONObject.getInt("Delta_hour"));
            Log.i(TAG,"" + roomJSONObject.getInt("Delta_minute"));


            RoomLog log = new RoomLog();
            log.setEnterTime(roomJSONObject.getString("Enter_Time"));
            log.setExitTime(roomJSONObject.getString("Exit_Time"));
            log.setDeltaHour(roomJSONObject.getInt("Delta_hour"));
            log.setDeltaMin(roomJSONObject.getInt("Delta_minute"));

            logs.add(log);
        }

    }


}
