package com.cmpe123.lock_management_system.ucsclocks;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.annotation.TargetApi;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;


public class LabsActivity extends AppCompatActivity implements MyAdapter.OnItemClicked {
    private static final String TAG = "LabsActivity";
    private TextView cruzID_tv;
    private RecyclerView mRecyclerView;
    private MyAdapter mAdapter;
    private View mProgressView;
    private RecyclerView.LayoutManager mLayoutManager;
    private Boolean loggedIn;
    private String userID = null;

    private List<Room> mRooms = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_labs);
        cruzID_tv =findViewById(R.id.cruziD_tv);
        mRecyclerView = findViewById(R.id.mRecyckleView);
        mProgressView = findViewById(R.id.login_progress);

        /*Check to make sure the user is logged in*/
        final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        /*LOGIN ACTIVITY*/
        loggedIn = prefs.getBoolean("LoggedIn", false);
        if(!loggedIn){
            Intent intent = new Intent(this, LoginActivity.class);
            startActivity(intent);
        }

        // use a linear layout manager
        mLayoutManager = new LinearLayoutManager(this);
        mRecyclerView.setLayoutManager(mLayoutManager);

    }

    /*Gets the click for an item on the recycler view*/
    @Override
    public void onItemClick(int position) {
        Room room = mRooms.get(position);
        Log.i(TAG,mRooms.get(position).toString());
        Intent intent = new Intent(this, RoomLogActivity.class);
        intent.putExtra("Room",room.getRoomNumber());
        startActivity(intent);

    }




    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    MenuInflater inflater = getMenuInflater();
    inflater.inflate(R.menu.menu, menu);
    return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    // Handle item selection
    switch (item.getItemId()) {
      case R.id.logout:
        logout();
        return true;
        case R.id.refresh:
            SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
            userID = prefs.getString("cruzId", null);
            if(userID != null) {
                new FetchRoomsTask().execute(userID);
                showProgress(true);
            }
      default:
        return super.onOptionsItemSelected(item);
    }
    }

    private void logout() {
        mRooms.clear();
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        prefs.edit().putBoolean("LoggedIn", false).commit();
        AccountStorage.SetAccount(this,null);
        Intent intent = new Intent(getApplicationContext(), LoginActivity.class);
        startActivity(intent);
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onResume() {
      super.onResume();
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        userID = prefs.getString("cruzId", null);
        if(userID != null) {
            cruzID_tv.setText("CruzID: " + userID);
            new FetchRoomsTask().execute(userID);
            showProgress(true);
        }

    }


    private class FetchRoomsTask extends AsyncTask<String,Void,List<Room>>{

        @Override
        protected List<Room> doInBackground(String... cruzID) {
            return new AppEngineFetchr().fetchItems(cruzID[0]);

        }

        @Override
        protected void onPostExecute(List<Room> items){
            mRooms = items;
            setUpAdapter();
            showProgress(false);
        }


    }

    /*Sets up adapter for the Recylerview*/
    private void setUpAdapter() {
        mAdapter = new MyAdapter(mRooms);
        mRecyclerView.setAdapter(mAdapter);
        mAdapter.setOnClick(this);
    }


    /*Display the progress wheel for when the labs are loading*/
    @TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
    private void showProgress(final boolean show) {
        // On Honeycomb MR2 we have the ViewPropertyAnimator APIs, which allow
        // for very easy animations. If available, use these APIs to fade-in
        // the progress spinner.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR2) {
            int shortAnimTime = getResources().getInteger(android.R.integer.config_shortAnimTime);

            mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);
            mProgressView.animate().setDuration(shortAnimTime).alpha(
                    show ? 1 : 0).setListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);
                }
            });
        } else {
            // The ViewPropertyAnimator APIs are not available, so simply show
            // and hide the relevant UI components.
            mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);

        }
    }

}
