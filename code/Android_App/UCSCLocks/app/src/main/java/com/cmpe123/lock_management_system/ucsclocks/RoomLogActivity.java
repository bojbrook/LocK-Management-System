package com.cmpe123.lock_management_system.ucsclocks;

import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;

import java.util.ArrayList;
import java.util.List;

public class RoomLogActivity extends AppCompatActivity {

    private List<RoomLog> mLogs= new ArrayList<>();
    private RecyclerView mRecyclerView;
    private logViewAdapter mAdapter;
    private RecyclerView.LayoutManager mLayoutManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_room_log);
        mRecyclerView = (RecyclerView) findViewById(R.id.RV_RoomLogs);
        String roomNumber = getIntent().getStringExtra("Room");
        setTitle(roomNumber);
        String cruzID = AccountStorage.getAccount(this);
        userInfo info = new userInfo(cruzID,roomNumber);
        new FetchRoomsTask().execute(info);

        // use a linear layout manager
        mLayoutManager = new LinearLayoutManager(this);
        mRecyclerView.setLayoutManager(mLayoutManager);

    }


    private class FetchRoomsTask extends AsyncTask<userInfo,Void,List<RoomLog>> {

        @Override
        protected List<RoomLog> doInBackground(userInfo... info) {
            return new AppEngineFetchr().fetchLogs(info[0].getCruzID(),info[0].getRoomNumber());

        }

        @Override
        protected void onPostExecute(List<RoomLog> items){
            mLogs = items;
            setUpAdapter();
        }


    }

    private void setUpAdapter() {
        mAdapter = new logViewAdapter(mLogs);
        mRecyclerView.setAdapter(mAdapter);
    }

    private class userInfo{
        private String cruzID;
        private String roomNumber;
        public userInfo(String cruzID, String roomNumber){
            this.cruzID = cruzID;
            this.roomNumber = roomNumber;

        }
        public String getCruzID() {
            return cruzID;
        }

        public String getRoomNumber() {
            return roomNumber;
        }
    }

}
