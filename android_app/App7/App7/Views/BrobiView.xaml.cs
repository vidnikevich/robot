
using App7.ViewModels;
using System;
using Xamarin.Forms;

namespace App7.Views
{
    public partial class BrobiView : ContentPage
    {
        public BrobiView()
        {
            InitializeComponent();
        }

        public void lbfPressed(object sender, EventArgs args)
        {
            ((BrobiViewModel)BindingContext).Left = 1;
        }

        public void lbfReleased(object sender, EventArgs args)
        {
            ((BrobiViewModel)BindingContext).Left = 0;
        }

        public void lbbPressed(object sender, EventArgs args)
        {
            ((BrobiViewModel)BindingContext).Left = 2;
        }

        public void lbbReleased(object sender, EventArgs args)
        {
            ((BrobiViewModel)BindingContext).Left = 0;
        }

        public void rbfPressed(object sender, EventArgs args)
        {
            ((BrobiViewModel)BindingContext).Right = 1;
        }

        public void rbfReleased(object sender, EventArgs args)
        {
            ((BrobiViewModel)BindingContext).Right = 0;
        }

        public void rbbPressed(object sender, EventArgs args)
        { 
            ((BrobiViewModel)BindingContext).Right = 2;
        }

        public void rbbReleased(object sender, EventArgs args)
        {
            ((BrobiViewModel)BindingContext).Right = 0;
        }
    }
}
